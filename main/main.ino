/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"
#include "pid.h"

PID pitchPID = PID(
  0,
  1.0,
  0.0,
  0.0,
  500,
  -20,
  20,
  PID_DIRECTION_DIRECT
);

//Gimbal g = Gimbal(PA9, PA10);

/* SETUP */
void setup() {
  Serial.begin(9600);
  Serial.println("Hello Lukas");

  setup_led();
  setup_sd();
  setup_sensors();

  //g.setCoordinates(50, 50);

  set_led(0, 255, 0);
}

/* LOOOOP */
void loop() {
  Serial.println("Hello Johann");
  update_sensors();
  float outX = pitchPID.Run(datapoint.gyro.pitch);
  Serial.println(outX);
  //g.setCoordinates((int)outX, 50);
}
