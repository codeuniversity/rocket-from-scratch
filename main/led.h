#pragma once

#define LED_RED_PIN PA2
#define LED_GREEN_PIN PA1
#define LED_BLUE_PIN PA0

void setup_led() {
  pinMode(LED_RED_PIN, OUTPUT); // red
  pinMode(LED_GREEN_PIN, OUTPUT); // green
  pinMode(LED_BLUE_PIN, OUTPUT); // blue
}

void set_led(int red, int green, int blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}
