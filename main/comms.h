#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa_STM32.h>



bool setup_comms() {
  LoRa.setPins(B12, C13, A1)

  return LoRa.begin(FREQUENCY);
}

void send_data(char const * data) {
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
}
