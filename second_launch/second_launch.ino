/* HEADERS */
#include <SPI.h>
#include <SD.h>

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>

/* MACROS */
#define PRINT_VALUE(value)                       \
  Serial.print(value); Serial.print(",");        \
  DATA_FILE.print(value); DATA_FILE.print(",");

#define PRINTLN_VALUE(value)                     \
  Serial.println(value);                         \
  DATA_FILE.println(value);                      \
  DATA_FILE.flush();


/* DATA STRUCTURES */
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
  long time;

  // orientation in degrees/s²
  struct Gyro {
    float x;
    float y;
    float z;
  } gyro;

  // acceleration in m/s²
  struct Acc {
    float x;
    float y;
    float z;
  } acc;

  // pressure in mbar
  float pressure;

  // temperature of the MS5611 sensor in Celsius
  float temperatureMS;

  // height in m
  float height;

  // height filtered through kalman filter
  float filtered_height;
} datapoint;


/* GLOBALBS */
// global FILE-objects for SD access
 File LOG_FILE;
 File DATA_FILE;

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
  String data_file = "-data.csv";
  String log_file = "-log.txt";
  // TODO: Is this pin correct?
  const int SD_PORT = 10;

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PORT)) {
    Serial.println("SD initialization failed!");
    while (true) {}
  }
  Serial.println("SD initialization done.");

  File number_file = SD.open("fileidx.txt", FILE_WRITE);
  int pos = number_file.position();
  number_file.write('e');
  number_file.close();

  data_file = pos + data_file;
  log_file = pos + log_file;

  DATA_FILE = SD.open(data_file, FILE_WRITE);
  LOG_FILE = SD.open(log_file, FILE_WRITE);

  Serial.println(data_file + " " + log_file);
}

void setup_sensors() {
  print_log("MS5611 ");
  print_log(MS5611.begin() ? "found" : "not found");

  mpu6050.begin();
  /* mpu6050.calcGyroOffsets(true); */
  // only relevant to the GY-86
  mpu6050.setGyroOffsets(-0.83,-1.56,0.15);

  /* DATA_FILE.println("Time, TempMPU, TempMS, Pressure, heightTP, heightKalman, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroZ, AngleX, AngleY, AngleZ"); */
  DATA_FILE.println("Time, GyroX, GyroY, GyroZ, AccX, AccY, AccZ, Pressure, TempMS, Height, KalHeight");
  DATA_FILE.flush();
}



/* LOOOOP */
void loop() {
  update_sensors();

  // if emergency() {
  //   ...
  // }

  // uncommenting this adds 8% of memory usage
  /* switch (STATE) { */
  /*   case State::Boot: */
  /*     STATE = State::Ready; */
  /*     set_led(STATE); */
  /*     print_log("Ready for liftoff! Start \"Ready\""); */
  /*   case State::Ready: */
  /*     if (datapoint.acc.z >= 240) { */
  /*       STATE = State::Flight; */
  /*       print_log("Detected liftoff. Start \"Flight\""); */
  /*     } */
  /*     break; */
  /*   case State::Flight: */
  /*     if (datapoint.acc.z <= -240) { */
  /*       STATE = State::Fall; */
  /*       print_log("Detected apogee. Start \"Fall\""); */
  /*     } */
  /*     break; */
  /*   case State::Fall: */
  /*     if (datapoint.filtered_height <= 10) { */
  /*       STATE = State::Chute; */
  /*       print_log("Eject parachute. Start \"Chute\""); */
  /*     } */
  /*     break; */
  /*   case State::Chute: */
  /*     if (datapoint.acc.z <= -240) { */
  /*       STATE = State::Land; */
  /*       print_log("Detected landing. Start \"Land\""); */
  /*     } */
  /*     break; */
  /*   case State::Land: */
  /*   case State::Error: */
  /*     set_led(STATE); */
  /*     while (true) {} */
  /* } */
}


/* HELPERS */
// prints all data from the Data struct to file and serial
void print_data() {
  PRINT_VALUE(datapoint.time
  PRINT_VALUE(datapoint.gyro.x);
  PRINT_VALUE(datapoint.gyro.y);
  PRINT_VALUE(datapoint.gyro.z);
  PRINT_VALUE(datapoint.acc.x);
  PRINT_VALUE(datapoint.acc.y);
  PRINT_VALUE(datapoint.acc.z);
  PRINT_VALUE(datapoint.pressure);
  PRINT_VALUE(datapoint.temperatureMS);
  PRINT_VALUE(datapoint.height);
  PRINTLN_VALUE(datapoint.filtered_height);
}

// print one logging statement to logfile and serial
void print_log(String && msg) {
  // print to serial monitor
  Serial.println(msg);

  // print to file
  LOG_FILE.print(millis());
  LOG_FILE.print(": ");
  LOG_FILE.println(msg);
  LOG_FILE.flush();
}

// TODO: check if pins (i.e., colours) are correct
// set status-LED based on state of flight
void set_led(State state) {
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


/* SENSORS */
// read one datapoint, filter bad values, do precalculations and log datapoint
void update_sensors() {
  mpu6050.update();

  int err = MS5611.read();
  if (err != MS5611_READ_OK) {
    print_log("Error in read:");
    print_log("err");
    return;
  }

  datapoint.time = millis();
  datapoint.acc.x = mpu6050.getAccX();
  datapoint.acc.y = mpu6050.getAccY();
  datapoint.acc.z = mpu6050.getAccZ();
  datapoint.gyro.x = mpu6050.getGyroX();
  datapoint.gyro.y = mpu6050.getGyroY();
  datapoint.gyro.z = mpu6050.getGyroZ();

  datapoint.temperatureMS = MS5611.getTemperature();
  datapoint.pressure = MS5611.getPressure();
  datapoint.height = calc_height(datapoint.temperatureMS, datapoint.pressure);

  kalman_estimate_height();

  print_data();
  /* print_log("Wrote sensor data to file"); */
}

float calc_height(float temp, float pressure) {
  const float P0 = 1013.25; // Average Pressure at sea level

  return ((pow((P0 / pressure), (1/5.257)) - 1) * (-1) * (temp + 273.15)) / 0.0065;
}

void kalman_estimate_height() {
  static float varHeight = 0.158;  // noice variance determined using excel and reading samples of raw sensor data
  static float varProcess = 1e-10;
  static float pred_est_cov= 0.0;
  static float Kalman_Gain = 0.0;
  static float est_cov = 1.0;
  static float mesurement_estimate_t_minus = 0.0;
  static float Zp = 0.0;
  static float mesurement_estimate_height = 0.0;

  pred_est_cov = est_cov + varProcess;
  Kalman_Gain = pred_est_cov/(pred_est_cov + varHeight);
  est_cov = (1-Kalman_Gain)*pred_est_cov;
  mesurement_estimate_t_minus = mesurement_estimate_height;
  Zp = mesurement_estimate_t_minus;
  mesurement_estimate_height = Kalman_Gain*(datapoint.height-Zp)+mesurement_estimate_t_minus;

  datapoint.filtered_height = mesurement_estimate_height;
}
