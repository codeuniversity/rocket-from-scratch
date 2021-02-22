#include <SPI.h>
#include <SD.h>

using namespace std;

enum class State {
  Boot,
  Flight,
  Fall,
  Chute,
  Land,
};
State state;

enum class LED_Mode {
  Ready,
  Error,
};

// TODO: add system to generate flexible filenames
const String LOG_FILE = "log.txt";
const String DATA_FILE = "data.csv";

// TODO: keep a number of values in memory, but not more
// TODO: use SD card code
// - [ ] file 1: data
// - [x] file 2: log (status, errors)

void setup() {
  // setup
  Serial.begin(9600);
  setup_led();
  setup_sd();
  setup_sensors();

  // we are ready to liftoff
  state = State::Boot;
  log_line("Start \"Boot\"");
  set_led(LED_Mode::Ready);
}

void loop() {
  int data = read_sensors(); // TODO: make this a struct or class

  // if emergency() {
  //   ...
  // }

  switch (state) {
    case State::Boot:
      if (data >= 995) {
        state = State::Flight;
        log_line("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data >= 995) {
        state = State::Fall;
        log_line("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data >= 995) {
        state = State::Chute;
        log_line("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data >= 995) {
        state = State::Land;
        log_line("Detected landing. Start \"Land\"");
      }
      break;
    case State::Land:
      while (true) {}
      break;
  }
}

void log_line(String msg) {
  // add timestamp to message
  msg = String(millis()) + ": " + msg;

  // print to serial
  Serial.println(msg);

  // print to logfile
  File file = SD.open(LOG_FILE, FILE_WRITE);
  file.println(msg);
  file.close();
}

// TODO: use mpu6050 code
int read_sensors() {
  int j = random(1000);
  return j;
}

void set_led(LED_Mode mode ) {
  switch (mode) {
    case LED_Mode::Ready:
      analogWrite(5, 255);
      break;
    case LED_Mode::Error:
      analogWrite(5, 0);
      analogWrite(3, 0);
      analogWrite(6, 255);
      break;
  }
}

void setup_led() {
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
}

void setup_sd() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    log_line("SD initialization failed!");
    set_led(LED_Mode::Error);
    while (true);
  }
  log_line("SD initialization done.");
}

void setup_sensors() {
  randomSeed(analogRead(0));
}
