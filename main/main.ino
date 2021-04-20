/* HEADERS */
//#include "led.h"
#include "sd.h"
#include "sensors.h"
#include "comms.h"

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
  STATE = State::Boot;
  Serial.begin(9600);

  if (setup_sd() == false)
  {
    print_log("Detected landing. Start \"Land\"");
    STATE = State::Error;
  };

  setup_sensors();
  if (setup_comms() == false)
  {
        STATE = State::Error;
  }

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
    if (datapoint.estimated_altitude_average >= 10.0)
    {
      STATE = State::Flight;
      last_height = datapoint.estimated_altitude_average;
      print_log("Detected liftoff. Start \"Flight\"");
    }
    break;
  case State::Flight:
    //compares mainloop's kalman height reading to the previous
    if (datapoint.estimated_altitude_average < last_height)
    {
      //TODO: fire recovery here
      STATE = State::Chute;
      print_log("Detected apogee. Start \"Chute\"");
    }
    last_height = datapoint.estimated_altitude_average;
    break;
  case State::Chute:
  //adjust 0 on day of the launch
    if (datapoint.estimated_altitude_average < 0.0)
    {
      STATE = State::Land;
      print_log("Detected landing. Start \"Land\"");
    }
    break;
  case State::Land:
    break;
  case State::Error:
    //    set_led(250,0,0);
    print_log("Error");
    break;
   default:
   print_log("No");
  }
}
