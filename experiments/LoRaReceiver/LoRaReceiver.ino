#include <SPI.h>
#include <LoRa.h>

#define FREQUENCY 868E6

#define BLUE 2
#define GREEN 3
#define RED 4

void setup() {
  Serial.begin(19200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  while (!Serial);

  Serial.println("LoRa Receiver");

  while (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    digitalWrite(RED, HIGH);
  }
  Serial.println("Success!");
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
