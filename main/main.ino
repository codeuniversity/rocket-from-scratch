/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"

/*DATA STRUCTURES*/
// `State` represents all states of the flight and has an additional "Boot" and "Error" state
enum class State
{
  Boot,
  Ready,
  Flight,
  Fall,
  Chute,
  Land,

  // something went wrong
  Error,
} STATE;

/* SETUP */
void setup()
{
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();

  set_led(0, 255, 0);
}

/* LOOOOP */
void loop()
{
  update_sensors();

  // if emergency() {
  //   ...
  // }

  // TODO: indicate state with LED
  switch (STATE)
  {
  case State::Boot:
    STATE = State::Ready;
    print_log("Ready for liftoff! Start \"Ready\"");
  case State::Ready:
    if (datapoint.acc.z >= 240)//check these triggers
    {
      STATE = State::Flight;
      print_log("Detected liftoff. Start \"Flight\"");
    }
    break;
  case State::Flight:
    if (datapoint.acc.z <= -240) //check these triggers
    {
      STATE = State::Fall;
      print_log("Detected apogee. Start \"Fall\"");
    }
    break;
  case State::Fall:
    if (datapoint.filtered_height <= 10) //check these triggers
    {
      STATE = State::Chute;
      print_log("Eject parachute. Start \"Chute\"");
    }
    break;
  case State::Chute:
    if (datapoint.acc.z <= -240) //check these triggers
    {
      STATE = State::Land;
      print_log("Detected landing. Start \"Land\"");
    }
    break;
  case State::Land:
  case State::Error:

  }
}
