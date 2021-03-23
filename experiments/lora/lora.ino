#include <LoRa.h>
#include <SPI.h>

#define BLUE 2
#define GREEN 3
#define RED 4

#define FREQUENCY 868E6

// NOTE:
// LED in pin D2 blinks fast (2/s) on error and slow (1/s) on success

//void blink (int duration = 500, int times = -1) {
//    for (int i = 0; i != times; i++) {
//        digitalWrite(LED, HIGH);
//        delay(duration);
//        digitalWrite(LED, LOW);
//        delay(duration);
//    }
//}

void setup() {
    Serial.begin(9600);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);


    Serial.print("Setting up LoRa module...");
    while (! LoRa.begin(FREQUENCY)) {
        Serial.println("ERROR");
//        blink(250, 1);
          digitalWrite(RED, HIGH);
    }
    Serial.println("SUCCESS");
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
}

void loop() {
   
}
