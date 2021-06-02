#pragma once

#include <MPU6050_light.h>
#include <MS5611.h>
#include <Wire.h>
#include <cppQueue.h>
#include "sd.h"
#include "comms.h"

static float in = 0;
static float sum = 0;
//size_queue is our main setting we can change to adjust the sensitivity of the estimated height in order to avoid accidental triggers.
//When doing low speed / on or near ground tests it should be set higher than during rocket flight.
int size_queue = 25;
bool firstIteration = true;
float heightOffset = 0;
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
} datapoint;

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

void setup_sensors() {

  q.push(&in);

  print_log("MS5611 ");
  print_log(MS5611.begin() ? "found" : "not found");

  // Configure the mpu6050 here. Look Ch. 6: https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
  mpu6050.begin(1, 3);
  mpu6050.setAccOffsets(0.05, -0.15, -0.15);
  mpu6050.setGyroOffsets(-0.93, -1.26, 0.25);

  /* DATA_FILE.println("Time, TempMPU, TempMS, Pressure, heightTP, heightKalman, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroZ, AngleX, AngleY, AngleZ"); */
  DATA_FILE.println("Time, GyroX, GyroY, GyroZ, AccX, AccY, AccZ, Pressure, TempMS, Height, EstHeight");
  DATA_FILE.flush();
}

float calc_height(float temp, float pressure) {
  // change these on the day
  const float P0 = 1003; // Sea level pressure in Berlin
  temp = 5; // Temperature in Berlin
  float calculatedHeight = ((pow((P0 / pressure), (1 / 5.257)) - 1) * (temp + 273.15)) / 0.0065;

  if(firstIteration){
    heightOffset = calculatedHeight;
    firstIteration = false;
  }

  return calculatedHeight - heightOffset;
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
    print_log("MS5611");
    return;
  }

  datapoint.time = millis();
  datapoint.acc.x = mpu6050.getAccX();
  datapoint.acc.y = mpu6050.getAccY();
  datapoint.acc.z = mpu6050.getAccZ();
  datapoint.gyro.x = mpu6050.getAngleX();
  datapoint.gyro.y = mpu6050.getAngleY();
  datapoint.gyro.z = mpu6050.getAngleZ();

  datapoint.temperatureMS = MS5611.getTemperature();
  datapoint.pressure = MS5611.getPressure();
  datapoint.height = calc_height(datapoint.temperatureMS, datapoint.pressure);
  datapoint.estimated_altitude_average = height_average(datapoint.height);

  print_data();


  // send each sensor reading to ground
  send_value(TIME, datapoint.time);
  send_value(GYRO_X, datapoint.gyro.x);
  send_value(GYRO_Y, datapoint.gyro.y);
  send_value(GYRO_Z, datapoint.gyro.z);
  send_value(ACC_X, datapoint.acc.x);
  send_value(ACC_Y, datapoint.acc.y);
  send_value(ACC_Z, datapoint.acc.z);
  send_value(PRESSURE, datapoint.pressure);
  send_value(TEMPERATURE, datapoint.temperatureMS);
  send_value(HEIGHT, datapoint.height);
  send_value(ESTIMATED_ALTITUDE_AVERAGE, datapoint.estimated_altitude_average);

  /* print_log("Wrote sensor data to file"); */
}
