#include <MPU6050_tockn.h>
#include "MS5611.h"
#include <Wire.h>

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

long timer = 0;

void setup() {
  Serial.begin(9600);

  bool b = MS5611.begin();
  Serial.println(b ? "found" : "not found");

  // Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {
  mpu6050.update();

  if(millis() - timer > 10){
    int result = MS5611.read();
    if (result != MS5611_READ_OK)
    {
      Serial.print("Error in read: ");
      Serial.println(result);
    }
    else
    {
      // Temp - Celsius
      Serial.print(MS5611.getTemperature());Serial.print("\t");
      // Pressure - Milli Bar
      Serial.print(MS5611.getPressure());Serial.print("\t");
    }

    // Temp - Celsius
    Serial.print(mpu6050.getTemp());Serial.print("\t");
    // Acceleration - ?
    Serial.print(mpu6050.getAccX());Serial.print("\t");
    Serial.print(mpu6050.getAccY());Serial.print("\t");
    Serial.print(mpu6050.getAccZ());Serial.print("\t");

    // Gyro - ?
    Serial.print(mpu6050.getGyroX());Serial.print("\t");
    Serial.print(mpu6050.getGyroY());Serial.print("\t");
    Serial.print(mpu6050.getGyroZ());Serial.print("\t");

    // AccAngle - ?
    Serial.print(mpu6050.getAccAngleX());Serial.print("\t");
    Serial.print(mpu6050.getAccAngleY());Serial.print("\t");

    // GyroAngle - ?
    Serial.print(mpu6050.getGyroAngleX());Serial.print("\t");
    Serial.print(mpu6050.getGyroAngleY());Serial.print("\t");
    Serial.print(mpu6050.getGyroAngleZ());Serial.print("\t");

    // Angle - Degrees
    Serial.print(mpu6050.getAngleX());Serial.print("\t");
    Serial.print(mpu6050.getAngleY());Serial.print("\t");
    Serial.println(mpu6050.getAngleZ());Serial.print("\t");

    timer = millis();
  }

}
