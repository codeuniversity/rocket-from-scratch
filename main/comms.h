#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa_STM32.h>


enum DataIndex {
    TIME = 0,
    GYRO_X,
    GYRO_Y,
    GYRO_Z,
    ACC_X,
    ACC_Y,
    ACC_Z,
    PRESSURE,
    TEMPERATURE,
    HEIGHT,
    ESTIMATED_ALTITUDE_AVERAGE
};


bool setup_comms() {
//   LoRa.setPins(10, 9, 2)
  return LoRa.begin(FREQUENCY);
}

void send_data(unsigned char const * data, int len) {
  LoRa.beginPacket();
  LoRa.write(data, len);
  LoRa.endPacket();
}

void send_value(DataIndex index, float value) {
    float hash = value/(index + 1);
    LoRa.beginPacket();
    LoRa.print((char) index);
    LoRa.print(value);
    LoRa.print(hash);
    LoRa.endPacket();
}

void send_value(DataIndex index, long value) {
    send_value(index,(float)value);
}
