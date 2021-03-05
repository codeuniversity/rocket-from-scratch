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

File myFile;
//Naming Limitations: https://www.arduino.cc/en/Reference/SDCardNotes
char filename[] = "first.csv";

MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

long timer = 0;
long start_time = millis();

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("SD initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println("Time+, Temp,Pressure,AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroAngleZ, AngleX, AngleY, AngleZ");
    myFile.close();
    Serial.println("Able to open file.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening file.");
  }

  MS5611.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  Serial.println("Setup done");
}

void loop() {
  mpu6050.update();

  if(millis() - timer > 10){
    int result = MS5611.read();
    if (result != MS5611_READ_OK) {
      // maybe not do this, so we at least have the measurements of the MPU6050
      myFile = SD.open(filename, FILE_WRITE);
      myFile.print("Error in read: ");
      myFile.println(result);
      myFile.close(); 
    } else {
      writeLine(timer);
    }

    timer = millis();
  }
}

void writeLine(long timer){
  myFile = SD.open(filename, FILE_WRITE);

  myFile.print(timer);
  myFile.print(",");

  myFile.print(MS5611.getTemperature());
  myFile.print(",");
  myFile.print(MS5611.getPressure());
  myFile.print(",");

  myFile.print(mpu6050.getAccX());
  myFile.print(",");
  myFile.print(mpu6050.getAccY());
  myFile.print(",");
  myFile.print(mpu6050.getAccZ());
  myFile.print(",");

  // Gyro - ?
  myFile.print(mpu6050.getGyroX());
  myFile.print(",");
  myFile.print(mpu6050.getGyroY());
  myFile.print(",");
  myFile.print(mpu6050.getGyroZ());
  myFile.print(",");

  // AccAngle - ?
  myFile.print(mpu6050.getAccAngleX());
  myFile.print(",");
  myFile.print(mpu6050.getAccAngleY());
  myFile.print(",");

  // GyroAngle - ?
  myFile.print(mpu6050.getGyroAngleX());
  myFile.print(",");
  myFile.print(mpu6050.getGyroAngleY());
  myFile.print(",");
  myFile.print(mpu6050.getGyroAngleZ());
  myFile.print(",");

  // Angle - Degrees
  myFile.print(mpu6050.getAngleX());
  myFile.print(",");
  myFile.print(mpu6050.getAngleY());
  myFile.print(",");
  myFile.print(mpu6050.getAngleZ());
  myFile.print("\n");

  myFile.close(); 
}