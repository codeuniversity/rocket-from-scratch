#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa.h>

bool setup_comms() {
//   LoRa.setPins(10, 9, 2)

  return LoRa.begin(FREQUENCY);
}

void send_data(float sensor_data) {

  // send packet
  LoRa.beginPacket();
  LoRa.print(sensor_data);
  LoRa.endPacket();

  //delay(1000);
}
