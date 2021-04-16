/* HEADERS */
//#include "led.h"
#include "sd.h"
#include "sensors.h"

/*DATA STRUCTURES*/
// `State` represents all states of the flight and has an additional "Boot" and "Error" state
enum class State
{
  Boot,
  Ready,
  Flight,
  Chute,
  Land,

  // something went wrong
  Error,
} STATE;

/* SETUP */
void setup()
{
  Serial.begin(9600);

  if (setup_sd() == false)
  {
    STATE = State::Error;
  };

  setup_sensors();

  //  set_led(0, 255, 0); TODO implement LED setup
}
float last_height;
/* LOOOOP */
void loop()
{
  update_sensors();

  // TODO: indicate state with LED
  switch (STATE)
  {
  case State::Boot:
    STATE = State::Ready;
    print_log("Ready for liftoff! Start \"Ready\"");
  case State::Ready:
    // trigger when reaching 10m height
    // this likely detects launch late, but good enough for now
    if (datapoint.filtered_height >= 10.0)
    {
      STATE = State::Flight;
      last_height = datapoint.filtered_height;
      print_log("Detected liftoff. Start \"Flight\"");
    }
    break;
  case State::Flight:
    //compares mainloop's kalman height reading to the previous
    if (datapoint.filtered_height < last_height)
    {
      //TODO: fire recovery here
      STATE = State::Chute;
      print_log("Detected apogee. Start \"Chute\"");
    }
    last_height = datapoint.filtered_height;
    break;
  case State::Chute:
    if (datapoint.filtered_height < 0.0)
    {
      STATE = State::Land;
      print_log("Detected landing. Start \"Land\"");
    }
    break;
  case State::Land:
    break;
  case State::Error:
    //    set_led(250,0,0);
    break;
  }
}
