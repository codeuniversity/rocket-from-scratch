// TODO(1): adapt to actual pin
int FIRE_PIN = PB12;

void setup() {
  Serial.begin(9600);

  pinMode(FIRE_PIN, OUTPUT);
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, LOW); // turn led on
  Serial.println("Countdown start...");


  // TODO(2): is this countdown enough?
  // wait for 150 sec
  delay(150000);

  Serial.println("FIRE!");
  digitalWrite(FIRE_PIN, HIGH); // ignite igniter
  digitalWrite(PC13, HIGH);     // turn led off

  // turn pin off after 1 sec
  delay(1000);
  digitalWrite(FIRE_PIN, LOW);
}

void loop() {}
