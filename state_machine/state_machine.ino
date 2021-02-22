#include <SPI.h>
#include <SD.h>

enum class State {
  Boot,
  Ready,
  Flight,
  Fall,
  Chute,
  Land,

  // something went wrong
  Error,
} STATE;

File log_file;
File data_file;

struct Data {
  // acceleration in m/s²
  int accX;
  int accY;
  int accZ;

  // velocity in m/s
  int velX;
  int velY;
  int velZ;

  // height in cm
  int height;
};

// TODO: keep a number of values in memory, but not more

void setup() {
  setup_logging();
  setup_sensors();
}

void loop() {
  Data data = read_sensors();

  // if emergency() {
  //   ...
  // }

  switch (STATE) {
    case State::Boot:
      STATE = State::Ready;
      set_led(STATE);
      print_log("Ready for liftoff! Start \"Ready\"");
    case State::Ready:
      if (data.accZ >= 240) {
        STATE = State::Flight;
        print_log("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data.accZ <= -240) {
        STATE = State::Fall;
        print_log("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data.height <= 10) {
        STATE = State::Chute;
        print_log("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data.accZ <= -240) {
        STATE = State::Land;
        print_log("Detected landing. Start \"Land\"");
      }
      break;
    case State::Land:
    case State::Error:
      set_led(STATE);
      while (true) {}
  }
}

void print_data(String data_str) {
  print_impl(data_file, data_str, ", ");
}

void print_log(String msg) {
  print_impl(log_file, msg, ": ");
}

void print_impl(File file, String msg, String sep) {
  // add timestamp to message
  msg = String(millis()) + sep + msg;

  // print to serial
  Serial.println(msg);

  // print to file
  file.println(msg);
  file.flush();
}

// TODO: use mpu6050 code
Data read_sensors() {
  Data data;
  data.accX = random(500) - 250;
  data.accY = random(500) - 250;
  data.accZ = random(500) - 250;
  data.velX = random(500) - 250;
  data.velY = random(500) - 250;
  data.velZ = random(500) - 250;
  data.height = random(500);

  print_data(
    String(data.accX) + ", " + String(data.accY) + ", " + String(data.accZ) + ", " +
    String(data.velX) + ", " + String(data.velY) + ", " + String(data.velZ) + ", " +
    String(data.height)
  );

  return data;
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
    STATE = State::Error;
  }
  print_log("SD initialization done.");

  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);
}

void setup_sensors() {
  randomSeed(analogRead(0));
}
