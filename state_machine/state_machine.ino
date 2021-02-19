int i;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  // state 0
  while (true) {
    i = read_sensor();

    // condition to go to next state
    if (i > 95) {
      break;
    }
  }

  // stuff to do in-between states
  Serial.println("transition to state 1");

  // state 1
  while (true) {
    i = read_sensor();

    if (i < 5) {
      break;
    }
  }

  Serial.println("transition to state 2");

  // state 2
  while (true) {
    i = read_sensor();

    if (i < 5) {
      break;
    }
  }

  Serial.println("done");

  // done
  while (true) {}
}

int read_sensor() {
  int j = random(100);
  Serial.println(j);
  return j;
}
