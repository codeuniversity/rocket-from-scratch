#include <LoRa.h>
#include <SPI.h>

#define LED 2

#define FREQUENCY 868E6

// NOTE:
// LED in pin D2 blinks fast (2/s) on error and slow (1/s) on success

void blink (int duration = 500, int times = -1) {
    for (int i = 0; i != times; i++) {
        digitalWrite(LED, HIGH);
        delay(duration);
        digitalWrite(LED, LOW);
        delay(duration);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);

    Serial.print("Setting up LoRa module...");
    if (! LoRa.begin(FREQUENCY)) {
        Serial.println("ERROR");
        blink(250);
    }
    Serial.println("SUCCESS");
}

void loop() {
    blink();
}

