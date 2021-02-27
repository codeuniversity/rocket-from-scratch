/* INCLUDES */
#include <SPI.h>
#include <SD.h>

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>


/* MACROS */
// print one logging statement to logfile and serial
#define LOG(msg) {		\
  Serial.println(msg);          \
  log_file.print(millis());	    \
  log_file.print(": ");         \
  log_file.print(msg);          \
  log_file.flush();             \
}

/* TYPES */
// `State` represents all states of the flight and has an additional "Boot" and "Error" state
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

// `Data` represents one datapoint, measured by our sensors
struct Data {
  // time in ms
  int time;

  // acceleration in m/s²
  struct Acc {
    double x;
    double y;
    double z;
  } acc;

  // velocity in m/s
  struct Vel {
    double x;
    double y;
    double z;
  } vel;

  // height in m
  int height;
};


/* GLOBALS */
// data object to read from
Data data;

// global FILE-objects for SD access
File log_file;
File data_file;

// sensor
MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

// TODO: keep a number of data points in memory, but not more


/* SETUP */
void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();
}

void setup_led() {
  pinMode(6, OUTPUT); // red
  pinMode(5, OUTPUT); // green
  pinMode(3, OUTPUT); // blue
}

// connect to SD and create File-objects
void setup_sd() {
  const String DATA_FILE = "test_data.csv";
  const String LOG_FILE = "test_log.txt";
  const int SD_PORT = 10;				// What is this value?

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PORT)) {
    Serial.println("SD initialization failed!");
    STATE = State::Error;
  }
  LOG("SD initialization done.");

  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);
}

void setup_sensors() {
    Serial.print("MS5611 ");
    Serial.println(MS5611.begin() ? "found" : "not found");

    Serial.print("calibrating MPU6050...");
    mpu6050.calcGyroOffsets(true);
    Serial.println("Done");
    
    data_file.println("Time, TempMPU, TempMS, Pressure, heightTP, heightAcc, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroZ, AngleX, AngleY, AngleZ");
}

/* LOOP */
void loop() {
  static void * (* state) (void) = boot;
  update_sensors();
  state = ((void * (*) (void)) state) ();
  set_led(STATE);
  delay(10);
}

/* STATES */
void * boot() {
  STATE = State::Ready;
  LOG("Ready for liftoff! Start 'Ready'");
  return (void *) ready;
}
void * ready() {
  if (data.acc.z < 240) return (void *) ready;
  STATE = State::Flight;
  LOG("Detected liftoff. Start 'Flight'");
  return (void *) flight;
}
void * flight() {
  if (data.acc.z > -240) return (void *) flight;
  STATE = State::Fall;
  LOG("Detected apogee. Start 'Fall'");
  return (void *) fall;
}
void * fall() {
  if (data.height > 10) return (void *) fall;
  STATE = State::Land;
  LOG("Eject parachute. Start 'Chute'");
  return (void *) chute;
}
void * chute() {
  if (data.acc.z > -240) return (void *) chute;
  STATE = State::Land;
  LOG("Detected landing. Start 'Land'");
  return (void *) land;
}
void * land() {
  return (void *) land;
}
void * error() {
  STATE = State::Error;
  return (void *) error;
}


/* HELPERS */
void write_data(double const * data, int size) {
  for (int i = 0; i < size; i++) {
    if (i) {
      Serial.print(',');
      data_file.print(',');
    }
    Serial.print(data[i]);
    data_file.print(data[i]);
  }
  Serial.println();
  data_file.println();
  data_file.flush();
}

// read one datapoint, filter bad values, do precalculations and log datapoint
void update_sensors() {
  mpu6050.update();

  data.time = millis();
  data.acc.x = mpu6050.getAccX();
  data.acc.y = mpu6050.getAccY();
  data.acc.z = mpu6050.getAccZ();

  double temperatureMS = MS5611.getTemperature();
  double pressure = MS5611.getPressure();
  double heightTP = calc_height(temperatureMS, pressure);
  //double heightAcc = calcHeightAcc(upwardsAcc);
  double upwardsAcc = mpu6050.getAccX();

  data.height = heightTP;
  
  {
    double const data[] = {
      1.0 * millis(),
      mpu6050.getTemp(),
      temperatureMS,
      pressure, 
      heightTP, 
//    heightAcc, 
      upwardsAcc,
      mpu6050.getAccY(),
      mpu6050.getAccZ(),
      mpu6050.getGyroX(),
      mpu6050.getGyroY(),
      mpu6050.getGyroZ(),
      mpu6050.getAccAngleX(),
      mpu6050.getAccAngleY(),
      mpu6050.getGyroAngleX(),
      mpu6050.getGyroAngleY(),
      mpu6050.getGyroAngleZ(),
      mpu6050.getAngleX(),
      mpu6050.getAngleY(),
      mpu6050.getAngleZ()
    };
    write_data(data, 19);
  }
  LOG("Wrote sensor data to file");
}

float calc_height(float temp, float pressure) {
  const float P0 = 1013.25; // Average Pressure at sea level

  return ((pow((P0 / pressure), (1/5.257)) - 1) * (-1) * (temp + 273.15)) / 0.0065;
}

// set status-LED based on state of flight
void set_led(State state ) {
  switch (state) {
    case State::Ready:
      analogWrite(6, 0);
      analogWrite(5, 255);
      analogWrite(3, 0);
      break;
    case State::Error:
      analogWrite(6, 255);
      analogWrite(5, 0);
      analogWrite(3, 0);
      break;
  }
}
