#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>

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

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

float calc_height(float temp, float pressure) {
  // change these on the day
  const float P0 = 1019.5; // Sea level pressure in Berlin
  temp = 5; // Temperature in Berlin
  return ((pow((P0 / pressure), (1 / 5.257)) - 1) * (temp + 273.15)) / 0.0065;
}

void kalman_estimate_height() {
  static float varHeight = 0.158;  // noice variance determined using excel and reading samples of raw sensor data
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
