enum class State {
  Boot,
  Flight,
  Fall,
  Chute,
  Land,
};
State state;

// TODO: keep a number of values in memory, but not more
// TODO: use SD card code
// * file 1: data
// * file 2: log (status, errors)

void setup() {
  Serial.begin(9600);
  setup_sensors();

  state = State::Boot;
  Serial.println("Start \"Boot\"");
}

void loop() {
  int data = read_sensor(); // TODO: make this a struct or class 
  
  // if emergency() {
  //   ...
  // }
  
  switch (state) {
    case State::Boot:
      if (data >= 995) {
        state = State::Flight;
        Serial.println("\nDetected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data >= 995) {
        state = State::Fall;
        Serial.println("\nDetected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data >= 995) {
        state = State::Chute;
        Serial.println("\nEject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data >= 995) {
        state = State::Land;
        Serial.println("Detected landing. Start \"Land\"");
      }
      break;
    case State::Land:
      while (true) {}
      break;
  }
}

void setup_sensors() {
  randomSeed(analogRead(0));
}

// TODO: use mpu6050 code
int read_sensor() {
  int j = random(1000);
  Serial.print(j); Serial.print(" ");
  return j;
}
