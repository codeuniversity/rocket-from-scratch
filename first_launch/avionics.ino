/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
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

File file;
//Naming Limitations: https://www.arduino.cc/en/Reference/SDCardNotes
char filename[] = "first.csv";

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

long timer = 0;
long start_time = millis();

void writeLine(long timer, File file){

  file.print(timer);
  file.print(",");

  file.print(MS5611.getTemperature());
  file.print(",");
  file.print(MS5611.getPressure());
  file.print(",");

  file.print(mpu6050.getAccX());
  file.print(",");
  file.print(mpu6050.getAccY());
  file.print(",");
  file.print(mpu6050.getAccZ());
  file.print(",");

  // Gyro - ?
  file.print(mpu6050.getGyroX());
  file.print(",");
  file.print(mpu6050.getGyroY());
  file.print(",");
  file.print(mpu6050.getGyroZ());
  file.print(",");

  // AccAngle - ?
  file.print(mpu6050.getAccAngleX());
  file.print(",");
  file.print(mpu6050.getAccAngleY());
  file.print(",");

  // GyroAngle - ?
  file.print(mpu6050.getGyroAngleX());
  file.print(",");
  file.print(mpu6050.getGyroAngleY());
  file.print(",");
  file.print(mpu6050.getGyroAngleZ());
  file.print(",");

  // Angle - Degrees
  file.print(mpu6050.getAngleX());
  file.print(",");
  file.print(mpu6050.getAngleY());
  file.print(",");
  file.print(mpu6050.getAngleZ());
  file.print("\n");
}

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  
  Serial.println("Initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  file = SD.open(filename, FILE_WRITE);

  if (file) {
    file.println("Time+, Temp,Pressure,AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroAngleZ, AngleX, AngleY, AngleZ");
    file.close();
    Serial.println("Able to open file.");
  } else {
    Serial.println("Error opening file.");
  }

  bool b = MS5611.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {
  file = SD.open(filename, FILE_WRITE);

  if (!file) {
    break;
  } else {
    loop {
      mpu6050.update();

      if(millis() - timer > 10){
        
        int result = MS5611.read();
        if (result != MS5611_READ_OK)
        {
          file.print("Error in read: ");
          file.println(result);
        }
        else
        {
          writeLine(timer, file);
        }
    
        timer = millis();
      }
    
      if (millis()-start_time > 300000){
        file.close();
        break;
      }
    }
  }
  break;
}
