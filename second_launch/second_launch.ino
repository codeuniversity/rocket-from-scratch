 #include <SPI.h>
 #include <SD.h>

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>

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

// global FILE-objects for SD access
 File log_file;
 File data_file;

// sensor
MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

// TODO: keep a number of data points in memory, but not more

// `Data` represents one datapoint, measured by our sensors
struct Data {
  // time in ms
  int time;

  // acceleration in m/s²
  struct Acc {
    float x;
    float y;
    float z;
  } acc;

  // velocity in m/s
  struct Vel {
    float x;
    float y;
    float z;
  } vel;

  // height in m
  float height;
} data;

struct KalmanFilter {
     double varHeight = 0.158;  // noice variance determined using excel and reading samples of raw sensor data
     double varProcess = 1e-8;
     double pred_est_cov= 0.0;
     double Kalman_Gain = 0.0;
     double est_cov = 1.0;
     double mesurement_estimate_t_minus = 0.0;
     double Zp = 0.0;
     double mesurement_estimate_height = 0.0;
   } kalmanFilter;

void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();
}

void loop() {
  update_sensors();
  kalman_estimate_height();

  // if emergency() {
  //   ...
  // }

  switch (STATE) {
    case State::Boot:
      STATE = State::Ready;
      set_led(STATE);
      print_log("Ready for liftoff! Start \"Ready\"");
    case State::Ready:
      if (data.acc.z >= 240) {
        STATE = State::Flight;
        print_log("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data.acc.z <= -240) {
        STATE = State::Fall;
        print_log("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      //if (data.height <= 10) {
      if (kalmanFilter.mesurement_estimate_height <= 10) {
        STATE = State::Chute;
        print_log("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data.acc.z <= -240) {
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

void write_data(double const * data, int size) {
  Serial.print(millis());
  data_file.print(millis());
  for (int i = 0; i < size; i++) {
    Serial.print(',');
    data_file.print(',');
    Serial.print(data[i]);
    data_file.print(data[i]);
  }
  Serial.println();
  data_file.println();
  data_file.flush();
}

// print one logging statement to logfile and serial
void print_log(String && msg) {
  // print to serial monitor
  Serial.println(msg);

  // print to file
  file.print(millis());
  file.print(": ");
  file.print(msg);
  file.flush();
}

// read one datapoint, filter bad values, do precalculations and log datapoint
void update_sensors() {
  mpu6050.update();

  int err = MS5611.read();
  if (err != MS5611_READ_OK) {
    Serial.print("Error in read: "); Serial.println(err);
  } else {
    data.height = calc_height(MS5611.getTemperature(), MS5611.getPressure());
  }

  data.time = millis();
  data.acc.x = mpu6050.getAccX();
  data.acc.y = mpu6050.getAccY();
  data.acc.z = mpu6050.getAccZ();

  double temperatureMS = MS5611.getTemperature();
  double pressure = MS5611.getPressure();
  double heightTP = calc_height(temperatureMS, pressure);
  double upwardsAcc = mpu6050.getAccX();

  data.height = heightTP;

  {
    double const data[] = {
      mpu6050.getTemp(),
      temperatureMS,
      pressure,
      heightTP,
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
    write_data(data, 18);
  }
  print_log("Wrote sensor data to file");
}

float calc_height(float temp, float pressure) {
  const float P0 = 1013.25; // Average Pressure at sea level

  return ((pow((P0 / pressure), (1/5.257)) - 1) * (-1) * (temp + 273.15)) / 0.0065;
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

void setup_led() {
  pinMode(6, OUTPUT); // red
  pinMode(5, OUTPUT); // green
  pinMode(3, OUTPUT); // blue
}

// connect to SD and create File-objects
void setup_sd() {
  String DATA_FILE = "-data.csv";
  String LOG_FILE = "-log.txt";
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

  DATA_FILE = pos + DATA_FILE;
  LOG_FILE = pos + LOG_FILE;
  
  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);

  Serial.println(DATA_FILE + " " + LOG_FILE);
}

void setup_sensors() {
  print_log("MS5611 ");
  print_log(MS5611.begin() ? "found" : "not found");

  mpu6050.begin();
  // TODO: replace this with setGyroOffset
  mpu6050.calcGyroOffsets(true);
    
  print_data("Time, TempMPU, TempMS, Pressure, heightTP, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroZ, AngleX, AngleY, AngleZ");
}

float kalman_estimate_height() {

    kalmanFilter.pred_est_cov = kalmanFilter.est_cov + kalmanFilter.varProcess;
    kalmanFilter.Kalman_Gain = kalmanFilter.pred_est_cov/(kalmanFilter.pred_est_cov + kalmanFilter.varHeight);
    kalmanFilter.est_cov = (1-kalmanFilter.Kalman_Gain)*kalmanFilter.pred_est_cov;
    kalmanFilter.mesurement_estimate_t_minus = kalmanFilter.mesurement_estimate_height;
    kalmanFilter.Zp = kalmanFilter.mesurement_estimate_t_minus;
    kalmanFilter.mesurement_estimate_height = kalmanFilter.Kalman_Gain*(data.height-kalmanFilter.Zp)+kalmanFilter.mesurement_estimate_t_minus;

    return kalmanFilter.mesurement_estimate_height;
  }
