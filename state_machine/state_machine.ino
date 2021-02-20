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
enum class LED_Mode {
  Ready,
  Error, 
};
State state;
//TODO: add system to generate flexible filenames
String logfile = "log.txt";
String datafile = "data.csv";

// TODO: keep a number of values in memory, but not more
// TODO: use SD card code
// * file 1: data
// * file 2: log (status, errors)

void setup() {
  Serial.begin(9600);
  setup_sd();
  setup_sensors();
  setup_led();
  state = State::Boot;
  log_line("Start \"Boot\"");
  set_led(LED_Mode::Ready);
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
        log_line("");
        log_line("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data >= 995) {
        state = State::Fall;
        log_line("");
        log_line("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data >= 995) {
        state = State::Chute;
        log_line("");
        log_line("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data >= 995) {
        state = State::Land;
        log_line("");
        log_line("Detected landing. Start \"Land\"");
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
void setup_sd() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    log_line("SD initialization failed!");
    set_led(LED_Mode::Error);
    while (true);
    //TODO: signal failed state with LED
  }
  log_line("SD initialization done.");
}

template<typename... Args>
void log_line(const char *msg, Args... args){
  char buffer[300];
  snprintf(buffer, 300, msg, args...);
  String bufferStr(buffer);
  String message = String(millis()) + " -> " + bufferStr;  
  Serial.println(message);
  File file = SD.open(logfile, FILE_WRITE);
  file.println(message);
  file.close();
}

// TODO: use mpu6050 code
int read_sensor() {
  int j = random(1000);
  return j;
}
void set_led(LED_Mode mode ) {
  switch (mode) {
    case LED_Mode::Ready:
      analogWrite(5, 255);
      break;
    case LED_Mode::Error:
      analogWrite(5,0);
      analogWrite(3,0);
      analogWrite(6,255);
      break;
  }
}
void setup_led() {
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
}

