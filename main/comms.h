#pragma once

#define FREQUENCY 868E6

#include <SPI.h>
#include <LoRa_STM32.h>

#define LORA_CS_PORT PA4

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
  LoRa.setPins(LORA_CS_PORT, PC13, PA1);
  return LoRa.begin(FREQUENCY);
}

  // start SPI connection to Lora module
  return LoRa.begin(FREQUENCY);
}

// Send one float `value` of type `index`
void send_value(DataIndex index, float value) {

    // calculate a hash of message:
    float hash = value / (index + 1);

    LoRa.beginPacket();
    LoRa.write((char) index);
    LoRa.write((unsigned char *) & value, sizeof(float));
    LoRa.write((unsigned char *) & hash, sizeof(float));
    LoRa.endPacket();
}
// Send one long `value` of type `index`
void send_value(DataIndex index, long value) {
    send_value(index, (float) value);
}
