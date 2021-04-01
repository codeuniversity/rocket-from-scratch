/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"

/* SETUP */
void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();

  set_led(0, 255, 0);
}

/* LOOOOP */
void loop() {
  update_sensors();
}
