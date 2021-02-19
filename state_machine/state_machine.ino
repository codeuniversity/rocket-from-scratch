enum class State {
  Boot,
  Flight,
  Fall,
  Chute,
  Land, 
};
State state;
int data;

void setup() {
  Serial.begin(9600);
  state = State::Boot;
  Serial.println("Start \"Boot\"");
  randomSeed(analogRead(0));
}

void loop() {
  data = read_sensor();
  
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

int read_sensor() {
  int j = random(1000);
  Serial.print(j); Serial.print(" ");
  return j;
}
