#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa_STM32.h>



bool setup_comms() {
  LoRa.setPins(PB12, PC13, PA1);

  while (!LoRa.begin(FREQUENCY));
  return true;
}

void send_data(char const * data) {
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  Serial.println("sending data");
  Serial.println(LoRa.parsePacket());
}
