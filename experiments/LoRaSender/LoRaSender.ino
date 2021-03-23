#include <SPI.h>
#include <LoRa.h>

#define FREQUENCY 868E6

#define BLUE 2
#define GREEN 3
#define RED 4

int counter = 0;

void setup() {
  Serial.begin(9600);
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
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
