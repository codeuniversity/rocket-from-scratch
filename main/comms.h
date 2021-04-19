#pragma once

#include <SPI.h>
#include <LoRa.h>

void setup_comms() {
//   LoRa.setPins(10, 9, 2)

  while (!LoRa.begin(868E6)) {
      return false;
  }
}

void send_data(sensor_data) {

  // send packet
  LoRa.beginPacket();
  LoRa.print(sensor_data);
  LoRa.endPacket();

  //delay(1000);
}
}
