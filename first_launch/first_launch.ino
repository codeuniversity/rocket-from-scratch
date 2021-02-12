/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

#include <MPU6050_tockn.h>
#include "MS5611.h"
#include <Wire.h>

File myFile;

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

long timer = 0;
long start_time = millis();

void setup() {

  // sensor set up
  Serial.begin(9600);
  // Open myFile communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  bool b = MS5611.begin();

  // Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  // SD card set up

  SD.begin(4);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("first_launch.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println("testing");
    // close the file:
    // myFile.close();
  } else {
    Serial.println("error opening first_launch.txt");
  }

 
}

void loop() {
  mpu6050.update();
 

  if(millis() - timer > 10){
    int result = MS5611.read();
    if (result != MS5611_READ_OK)
    {
      myFile.print("Error in read: ");
      myFile.println(result);
    }
    else
    {
      // Temp - Celsius
      myFile.print(MS5611.getTemperature());myFile.print("\t");
      // Pressure - Milli Bar
      myFile.print(MS5611.getPressure());myFile.println("\n");
    }

    // Temp - Celsius
    myFile.print(mpu6050.getTemp());myFile.print("\t");
    // Acceleration - ?
    myFile.print(mpu6050.getAccX());myFile.print("\t");
    myFile.print(mpu6050.getAccY());myFile.print("\t");
    myFile.print(mpu6050.getAccZ());myFile.print("\n");

    // Gyro - ?
    myFile.print(mpu6050.getGyroX());myFile.print("\t");
    myFile.print(mpu6050.getGyroY());myFile.print("\t");
    myFile.print(mpu6050.getGyroZ());myFile.print("\n");

    // AccAngle - ?
    myFile.print(mpu6050.getAccAngleX());myFile.print("\t");
    myFile.print(mpu6050.getAccAngleY());myFile.print("\n");

    // GyroAngle - ?
    myFile.print(mpu6050.getGyroAngleX());myFile.print("\t");
    myFile.print(mpu6050.getGyroAngleY());myFile.print("\t");
    myFile.print(mpu6050.getGyroAngleZ());myFile.print("\n");

    // Angle - Degrees
    myFile.print(mpu6050.getAngleX());myFile.print("\t");
    myFile.print(mpu6050.getAngleY());myFile.print("\t");
    myFile.print(mpu6050.getAngleZ());myFile.print("\n");

    timer = millis();
  }

  if (millis()-start_time > 300000){
    myFile.close();
  }

}
