#include <SPI.h>
#include <LoRa.h>


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

enum DataIndex {
    TIME = 0,
    GYRO_X,
    GYRO_Y,
    GYRO_Z,
    ACC_X,
    ACC_Y,
    ACC_Z,
    PRESSURE,
    TEMPERATURE,
    HEIGHT,
    ESTIMATED_ALTITUDE_AVERAGE
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  //LoRa.setPins(10, 9, 3);


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
    char packet [9];

    if (* packet = 0)
        print_value ((DataIndex) packet [0], * (long *) packet + 1);
    else
        print_value ((DataIndex) packet [0], * (float *) packet + 1);


    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}

void print_value (DataIndex index, long value) {
    Serial.print("time\t");
    Serial.println(value);
}
void print_value (DataIndex index, float value) {
    switch (index) {
        case GYRO_X:
            Serial.print("gyro_x\t");
            break;
        case GYRO_Y:
            Serial.print("gyro_y\t");
            break;
        case GYRO_Z:
            Serial.print("gyro_z\t");
            break;
        case ACC_X:
            Serial.print("acc_x\t");
            break;
        case ACC_Y:
            Serial.print("acc_y\t");
            break;
        case ACC_Z:
            Serial.print("acc_z\t");
            break;
        case PRESSURE:
            Serial.print("pressure\t");
            break;
        case TEMPERATURE:
            Serial.print("temperature\t");
            break;
        case HEIGHT:
            Serial.print("height\t");
            break;
        case ESTIMATED_ALTITUDE_AVERAGE:
            Serial.print("estimated_altitude_average\t");
            break;
        default:
            Serial.println("ERROR - invalid datapoint index");
            return;
    }
    Serial.println(value);
}
