/* HEADERS */
#include "led.h"
#include "sd.h"
#include "sensors.h"

/* SETUP */
void setup() {
  Serial.begin(9600);

  setup_led();
  setup_sd();
  setup_sensors();

  set_led(0, 255, 0);
}

void setup_sensors() {
  print_log("MS5611 ");
  print_log(MS5611.begin() ? "found" : "not found");

  mpu6050.begin();
  /* mpu6050.calcGyroOffsets(true); */
  // only relevant to the GY-86
  mpu6050.setGyroOffsets(-0.83, -1.56, 0.15);

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
