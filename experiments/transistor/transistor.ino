#define TRIGGER 4
#define SWITCH 7
#define LED 13

/**
 * Program for testing transistors.
 * The signal read from D4 is used to toggle the signal written to D7 which should be connected to the base of the transistor.
 * The onboard LED signals whether the trigger is on or off:
 * 
 * Setting the TRIGGER sets both LED and SWITCH correspondingly.
 */

void setup() {
    Serial.begin(9600);
    pinMode(TRIGGER, INPUT);
    pinMode(SWITCH, OUTPUT);

    Serial.println("Waiting for trigger (set D4 to HIGH)");
}

void loop() {
    while (digitalRead(TRIGGER) == LOW);

    Serial.println("Triggered!");
    digitalWrite(LED, HIGH);
    digitalWrite(SWITCH, HIGH);
    while (digitalRead(TRIGGER) == HIGH);

    digitalWrite(LED, LOW);
    digitalWrite(SWITCH, LOW);
    Serial.println("Release...");
}
