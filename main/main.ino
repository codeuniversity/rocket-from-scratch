/* HEADERS */
// #include "gimbal.h"
#include "led.h"
#include "sd.h"
#include "sensors.h"
#include "pid.h"

//PID pitchPID = PID(
//  90,  // target value
//  1.0, // kp -> proportional
//  0.0, // ki -> integral
//  0.0, // kd -> derivative
//  0,   // delay -> we only get a new value every x ms
//  0,   // min
//  100, // max
//  PID_DIRECTION_DIRECT // direction
//);

// Gimbal g = Gimbal(PA9, PA10);

/* SETUP */
void setup() {
  Serial.begin(9600);

  // setup_led();
  // setup_sd();
  setup_sensors();

  // g.setCoordinates(50, 50);

  // set_led(0, 255, 0);
}

/* LOOOOP */
void loop() {
  delay(1000);
  update_sensors();

  float pitch = datapoint.gyro.pitch;
  Serial.print("pitch: "); Serial.println(pitch);
  if (pitch > 180.0) pitch = 180.0;
  else if (pitch < 0.0) pitch = 0.0;
  int x = (pitch * 100.0) / 180.0;
  Serial.print("x: "); Serial.println(x);

  // g.setCoordinates(x, 50);
}
