#include <SPI.h>
#include <LoRa_STM32.h>


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

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    char * dataString = (char *) & datapoint;
    // received a packet
    Serial.print("Received packet '");

    // read packet
    for (int i = 0; i < packetSize; i++) {
      dataString[i] = LoRa.read();
    }

    Serial.println(datapoint.estimated_altitude_average);
    
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
