#include <SPI.h>
#include <SD.h>

using namespace std;

enum class State {
  Boot,
  Ready,
  Flight,
  Fall,
  Chute,
  Land,

  // something went wrong
  Error,
};
State state;

File log_file;
File data_file;

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
}

void loop() {
  int data = read_sensors(); // TODO: make this a struct or class

  // if emergency() {
  //   ...
  // }

  switch (state) {
    case State::Boot:
      state = State::Ready;
      set_led(state);
      log_line("Ready for liftoff! Start \"Ready\"");
    case State::Ready:
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
    case State::Error:
      set_led(state);
      while (true) {}
  }
}

void log_line(String msg) {
  // add timestamp to message
  msg = String(millis()) + ": " + msg;

  // print to serial
  Serial.println(msg);

  // print to logfile
  log_file.println(msg);
  log_file.flush();
}

// TODO: use mpu6050 code
int read_sensors() {
  int j = random(1000);
  return j;
}

void set_led(State state ) {
  switch (state) {
    case State::Ready:
      analogWrite(3, 0);
      analogWrite(5, 255);
      analogWrite(6, 0);
      break;
    case State::Error:
      analogWrite(3, 0);
      analogWrite(5, 0);
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
  const String DATA_FILE = "data.csv";
  const String LOG_FILE = "log.txt";
  const int SD_PORT = 10;

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PORT)) {
    Serial.println("SD initialization failed!");
    state = State::Error;
  }
  log_line("SD initialization done.");

  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);
}

void setup_sensors() {
  randomSeed(analogRead(0));
}
