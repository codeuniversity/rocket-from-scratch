/* HEADERS */
//#include "led.h"
//#include "sd.h"
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
  pinMode(PB10, OUTPUT);
  Serial.begin(9600);
  Serial.println("start serial");

//  if (!setup_sd())
//  {
//    //STATE = State::Error;
//    Serial.println("SD failed");
//    print_log("ERROR! SD \"Error\"");
//  };

  if (!setup_sensors()) {
    print_log("ERROR! Sensors \"Error\"");
    STATE = State::Error;
  }
  Serial.println("setup sensors worked");
  
  if (!setup_comms())
  {
    print_log("ERROR! Comms \"Error\"");
    STATE = State::Error;
  }
  Serial.println("setup comms worked");

  digitalWrite(PB10, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(PB10, LOW);    // turn the LED off by making the voltage LOW
  delay(500); 

  //  set_led(0, 255, 0); TODO implement LED setup
}
float last_height;
/* LOOOOP */
void loop()
{
  update_sensors();

  digitalWrite(PB10, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(PB10, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 

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
