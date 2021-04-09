#pragma once

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>

#include "sd.h"

// `Data` represents one datapoint, measured by our sensors
struct Data {
  // time in ms
  long time;

  // orientation in degrees/s²
  struct Gyro {
    float x;
    float y;
    float z;
    float pitch;
    float yaw;
    float roll;
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

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

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

float calc_height(float temp, float pressure) {
  // change these on the day
  const float P0 = 1019.5; // Sea level pressure in Berlin
  temp = 5; // Temperature in Berlin
  return ((pow((P0 / pressure), (1 / 5.257)) - 1) * (temp + 273.15)) / 0.0065;
}

void kalman_estimate_height() {
  static float varHeight = 0.158;  // noise variance determined using excel and reading samples of raw sensor data
  static float varProcess = 1e-6;
  static float pred_est_cov = 0.0;
  static float Kalman_Gain = 0.0;
  static float est_cov = 1.0;
  static float mesurement_estimate_t_minus = 0.0;
  static float Zp = 0.0;
  static float mesurement_estimate_height = 0.0;

  pred_est_cov = est_cov + varProcess;
  Kalman_Gain = pred_est_cov / (pred_est_cov + varHeight);
  est_cov = (1 - Kalman_Gain) * pred_est_cov;
  mesurement_estimate_t_minus = mesurement_estimate_height;
  Zp = mesurement_estimate_t_minus;
  //mesurement_estimate_height = Kalman_Gain*(datapoint.height-Zp)+mesurement_estimate_t_minus;
  mesurement_estimate_height = Kalman_Gain * (datapoint.height - Zp) + datapoint.height;

  datapoint.filtered_height = mesurement_estimate_height;
}

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
  datapoint.gyro.pitch = mpu6050.getAngleX();
  datapoint.gyro.yaw = mpu6050.getAngleZ();
  datapoint.gyro.roll = mpu6050.getAngleY();
  datapoint.gyro.y = mpu6050.getGyroY();
  datapoint.gyro.z = mpu6050.getGyroZ();

  datapoint.temperatureMS = MS5611.getTemperature();
  datapoint.pressure = MS5611.getPressure();
  datapoint.height = calc_height(datapoint.temperatureMS, datapoint.pressure);

  kalman_estimate_height();

  print_data();
  /* print_log("Wrote sensor data to file"); */
}
