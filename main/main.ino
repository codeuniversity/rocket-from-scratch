/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"

/* MACROS */
#define PRINT_VALUE(value)                       \
  Serial.print(value); Serial.print(",");        \
  DATA_FILE.print(value); DATA_FILE.print(",");

#define PRINTLN_VALUE(value)                     \
  Serial.println(value);                         \
  DATA_FILE.println(value);                      \
  DATA_FILE.flush();


/* SETUP */
void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();

  set_led(0, 255, 0);
}

// connect to SD and create File-objects
void setup_sd() {
  String data_file = "-data.csv";
  String log_file = "-log.txt";
  // TODO: Is this pin correct?

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("SD initialization failed!");
    set_led(255, 0, 0);
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
}


/* HELPERS */
// prints all data from the Data struct to file and serial
void print_data() {
  PRINT_VALUE(datapoint.time);
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
  // change these on the day
  const float P0 = 1019.5; // Sea level pressure in Berlin
  temp = 5; // Temperature in Berlin
  return ((pow((P0 / pressure), (1/5.257)) - 1) * (temp + 273.15)) / 0.0065;
}

void kalman_estimate_height() {
  static float varHeight = 0.158;  // noice variance determined using excel and reading samples of raw sensor data
  static float varProcess = 1e-6;
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
  //mesurement_estimate_height = Kalman_Gain*(datapoint.height-Zp)+mesurement_estimate_t_minus;
  mesurement_estimate_height = Kalman_Gain*(datapoint.height-Zp)+datapoint.height;

  datapoint.filtered_height = mesurement_estimate_height;
}
