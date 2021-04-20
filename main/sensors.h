#pragma once

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>
#include <cppQueue.h>
#include "sd.h"
#include "comms.h"

static float in = 0;
static float sum = 0;
int size_queue = 20;
cppQueue  q(sizeof(in), size_queue, FIFO);  // Instantiate queue

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
  float estimated_altitude_average;

  // null terminator needed to prevent array overflow
  char const O = 0;
} datapoint;

void send_data (Data const & data) {
    send_data ((char const *) & data);
}

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

void setup_sensors() {

  q.push(&in);
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



float height_average(float in){
  float out;
  q.push(&(in));
  if (q.getCount() < size_queue) {
    sum += in;
  }
  if (q.getCount() == size_queue) {
    sum += in;
    q.pop(&out);
    sum -= out;
    float average = sum /size_queue;
    return average;
    }
    else{
      return 0;
  }
}

// prints all data from the Data struct to file
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
  PRINTLN_VALUE(datapoint.estimated_altitude_average);
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

  datapoint.temperatureMS = MS5611.getTemperature();
  datapoint.pressure = MS5611.getPressure();
  datapoint.height = calc_height(datapoint.temperatureMS, datapoint.pressure);
  datapoint.estimated_altitude_average = height_average(datapoint.height);



  print_data();

  send_data(datapoint);
  /* print_log("Wrote sensor data to file"); */
}

