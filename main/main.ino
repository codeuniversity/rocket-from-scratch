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
  Serial.begin(9600);
  pinMode(PB9, OUTPUT);
  digitalWrite(PB9, HIGH);

  if (!setup_sd())
  {
    STATE = State::Error;
    print_log("ERROR! SD \"Error\"");
  };

  setup_sensors();
  if (!setup_comms())
  {
    STATE = State::Error;
    print_log("ERROR! Comms \"Error\"");
    for (int i = 0; i < 5; i++) {
      digitalWrite(PB9, HIGH);
      delay(200);
      digitalWrite(PB9, LOW);
      delay(1200);
    }

  while (STATE == State::Error);
  }

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
    if (datapoint.estimated_altitude_average >= 1.5)
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
    if (datapoint.estimated_altitude_average < 0.0)
    {
      STATE = State::Land;
      print_log("Detected landing. Start \"Land\"");
    }
    break;
  case State::Land:
    break;
  case State::Error:
    print_log("something didn't work good luck fixing it!");
    //    set_led(250,0,0);
    break;
  }
}
