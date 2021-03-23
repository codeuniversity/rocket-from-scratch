#include <SPI.h>
#include <LoRa.h>

#define FREQUENCY 868E6

#define BLUE 2
#define GREEN 3
#define RED 4

int counter = 0;

void setup() {
  Serial.begin(19200);
   pinMode(RED, OUTPUT);
   pinMode(GREEN, OUTPUT);

  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
    digitalWrite(RED, HIGH);
  }
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
  LoRa.receive();
  LoRa.onReceive([](int packetSize) {
    receivePong(packetSize);
    delay(1000);
    sendPing();
  });
}


void sendPing() {
  Serial.print("Sending ping packet: ");

  LoRa.beginPacket();
  LoRa.print("Pong: ");
  LoRa.print(counter++);
  LoRa.endPacket();
}

void receivePong(int packetSize) {
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

void loop() {

}
