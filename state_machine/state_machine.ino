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

void setup() {
  setup_logging();
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
      print_log("Ready for liftoff! Start \"Ready\"");
    case State::Ready:
      if (data >= 995) {
        state = State::Flight;
        print_log("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data >= 995) {
        state = State::Fall;
        print_log("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data >= 995) {
        state = State::Chute;
        print_log("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data >= 995) {
        state = State::Land;
        print_log("Detected landing. Start \"Land\"");
      }
      break;
    case State::Land:
    case State::Error:
      set_led(state);
      while (true) {}
  }
}

void print_data(String data_str) {
  print_impl(data_file, data_str);
}

void print_log(String msg) {
  print_impl(log_file, msg);
}

void print_impl(File file, String msg) {
  // add timestamp to message
  msg = String(millis()) + ": " + msg;

  // print to serial
  Serial.println(msg);

  // print to file
  file.println(msg);
  file.flush();
}

// TODO: use mpu6050 code
int read_sensors() {
  int j = random(1000);
  print_data(String(j));
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

void setup_logging() {
  Serial.begin(9600);
  setup_sd();
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
  print_log("SD initialization done.");

  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);
}

void setup_sensors() {
  randomSeed(analogRead(0));
}
