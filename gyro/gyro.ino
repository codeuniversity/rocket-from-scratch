#include<Wire.h>

const int MPU_addr = 0x68; // I2C address of the MPU-6050
float AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // Talk to the register 6B
  Wire.write(0);     // set zero into 6B register (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop() {
  read();
  print();
}

void read() {
  // Request data
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers

  // Read accelerometer data
  // For a range of +-2g, we need to divide by 16384, according to the datasheet
  AcX = (Wire.read() << 8 | Wire.read()) / 16384.0; // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = (Wire.read() << 8 | Wire.read()) / 16384.0; // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  // Read temperature data - not used atm
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  // Read gyroscope data
  // For a 250deg/s range we need to divide by 131.0, according to the datasheet
  GyX = (Wire.read() << 8 | Wire.read()) / 131.0; // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = (Wire.read() << 8 | Wire.read()) / 131.0; // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = (Wire.read() << 8 | Wire.read()) / 131.0; // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  // Correct the sensor data with the calculated error values
  GyX = GyX + 1.85; // GyroErrorX ~(-0.56)
  GyY = GyY + 0.15; // GyroErrorY ~(2)
  GyZ = GyZ - 0.15; // GyroErrorZ ~ (-0.8)

  accAngleX - 0.58; // AccErrorX ~(0.58)
  accAngleY + 1.58; // AccErrorY ~(-1.58)

  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(     AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI);

  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyY * elapsedTime;
  yaw =  yaw + GyZ * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
}

void print() {
  Serial.print("AcX: "); Serial.print(AcX);
  Serial.print(" | AcY: "); Serial.print(AcY);
  Serial.print(" | AcZ: "); Serial.println(AcZ);

  //Serial.print("accAngleX: "); Serial.print(accAngleX);
  //Serial.print(" | accAngleY: "); Serial.println(accAngleY);

  Serial.print("GyX: "); Serial.print(GyX);
  Serial.print(" | GyY: "); Serial.print(GyY);
  Serial.print(" | GyZ: "); Serial.println(GyZ);

  //Serial.print("roll: "); Serial.print(roll);
  //Serial.print(" | pitch: "); Serial.print(pitch);
  //Serial.print(" | yaw: "); Serial.println(yaw);
}
