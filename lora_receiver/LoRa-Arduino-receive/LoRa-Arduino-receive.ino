#include <SPI.h>
#include <LoRa.h>

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

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("Finished setup successfully");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // allocate space to write received packet to:
    char packet [packetSize];

    // read message into array
    for (int i = 0; i < packetSize; i++) {
      packet [i] = LoRa.read();
    }
    
    // read value from array and cast it to float
    // (NOTE: yes, `long` and `float` have the same size internally)
    float value = * (float *) (packet + 1);

    // calculate hash from value
    float hash = value /(* packet + 1);

    // read hash from array and cast it to float
    float check_hash = * (float *) (packet + 1 +  sizeof(float));

    //Serial.print("Index:\t"); Serial.println((int) * packet);
    //Serial.print("Value:\t"); Serial.println(value);
    //Serial.print("Hash:\t");  Serial.print  (check_hash); Serial.print(",\tshould be: "); Serial.println(hash);
    
    // check if calculated and retrieved hash match, i.e., if the message was corrupted
    if (check_hash != hash) {
      Serial.print("Check hash failed miserably with: ");
      Serial.println(value);
      return;
    }

    switch (* packet) {
        case TIME:
            Serial.print("time\t");
            Serial.println(* (long *) & value);
            return;
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
  
    //Serial.print(" with RSSI ");
    //Serial.println(LoRa.packetRssi());
  }
}
