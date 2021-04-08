/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"
#include "pid.h"
#include "gimbal.h"

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

Gimbal g;

/* SETUP */
void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();

  g = Gimbal(PA9, PA10);
  g.setCoordinates(50, 50);

  set_led(0, 255, 0);
}

/* LOOOOP */
void loop() {
  datapoint = update_sensors();
  outX = pitchPID.Run(datapoint.gyro.pitch);
  g.setCoordinates((int)outX, 50)
}
