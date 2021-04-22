// TODO(1): adapt to actual pin
int FIRE_PIN = LED_BUILTIN;

void setup() {
  Serial.begin(9600);

  pinMode(FIRE_PIN, OUTPUT);

  // TODO(2): is this countdown enough?
  // wait for 20 sec
  delay(20000);

  Serial.println("FIRE!");
  digitalWrite(FIRE_PIN, HIGH);
}

void loop() {}
