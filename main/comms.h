#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa_STM32.h>


// Index that tells us which sensor reading we're serialising
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
  // uncomment to change ss, reset and dio0 pin, respectively  
//   LoRa.setPins(10, 9, 2)

  // start SPI connection to Lora module
  return LoRa.begin(FREQUENCY);
}

// Send one float `value` of type `index`
void send_value(DataIndex index, float value) {

    // calculate a hash of message:
    float hash = value / (index + 1);

    LoRa.beginPacket();
    LoRa.print((char) index);                       // serialise data index as first byte
    LoRa.write((char *) & value, sizeof(float));    // serialise value as byte array
    LoRa.write((char *) & hash, sizeof(float));     // serialise hash as byte array
    LoRa.endPacket();
}
// Send one long `value` of type `index`
void send_value(DataIndex index, long value) {
    send_value(index, (float) value);
}
