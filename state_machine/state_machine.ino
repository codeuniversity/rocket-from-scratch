#include <SPI.h>
#include <SD.h>

#include <MPU6050_tockn.h>
#include <MS5611.h>
#include <Wire.h>

// `State` represents all states of the flight and has an additional "Boot" and "Error" state
enum class State {
  Boot,
  Ready,
  Flight,
  Fall,
  Chute,
  Land,

  // something went wrong
  Error,
} STATE;

// global FILE-objects for SD access
File log_file;
File data_file;

// sensor
MPU6050 mpu6050(Wire);
MS5611 MS5611(0x77);   // 0x76 = CSB to VCC; 0x77 = CSB to GND

// TODO: keep a number of data points in memory, but not more

// `Data` represents one datapoint, measured by our sensors
struct Data {
  // time in ms
  int time;

  // acceleration in m/s²
  int accX;
  int accY;
  int accZ;

  // velocity in m/s
  int velX;
  int velY;
  int velZ;

  // height in dm
  int height;
};

void setup() {
  Serial.begin(9600);

  setup_led();
  setup_logging();
  setup_sensors();
  delay(0);
}

void loop() {
  static Data data;
  data = read_sensors();
  delay(10);

  // if emergency() {
  //   ...
  // }

  /*

  switch (STATE) {
    case State::Boot:
      STATE = State::Ready;
      set_led(STATE);
      print_log("Ready for liftoff! Start \"Ready\"");
    case State::Ready:
      if (data.accZ >= 240) {
        STATE = State::Flight;
        print_log("Detected liftoff. Start \"Flight\"");
      }
      break;
    case State::Flight:
      if (data.accZ <= -240) {
        STATE = State::Fall;
        print_log("Detected apogee. Start \"Fall\"");
      }
      break;
    case State::Fall:
      if (data.height <= 10) {
        STATE = State::Chute;
        print_log("Eject parachute. Start \"Chute\"");
      }
      break;
    case State::Chute:
      if (data.accZ <= -240) {
        STATE = State::Land;
        print_log("Detected landing. Start \"Land\"");
      }
      break;
    case State::Land:
    case State::Error:
      set_led(STATE);
      while (true) {}
  }

  */
}

// print one datapoint to csv-file and serial
void print_data(String const & msg) {
  print_impl(data_file, msg, ", ");
}

// print one logging statement to logfile and serial
void print_log(String const & msg) {
  print_impl(log_file, msg, ": ");
}

void print_impl(File file, String const & msg, String const & sep) {
  // add timestamp to message

  // print to serial
  Serial.println(msg);

  // print to file
  file.print(millis());
  file.print(sep);
  file.print(msg);
  file.flush();
}

// read one datapoint, filter bad values, do precalculations and log datapoint
Data read_sensors() {
  static Data data;
  mpu6050.update();

  data.time = millis();
  data.accX = mpu6050.getAccX();
  data.accY = mpu6050.getAccY();
  data.accZ = mpu6050.getAccZ();

  double temperatureMS = MS5611.getTemperature();
  double pressure = MS5611.getPressure();
  double heightTP = calcHeightTP(temperatureMS, pressure);
  //double heightAcc = calcHeightAcc(upwardsAcc);
  double upwardsAcc = mpu6050.getAccX();

  data.height = heightTP;
  
  {
  String data = 
    String(mpu6050.getTemp()) + ", " +
    String(temperatureMS) + ", " +
    String(pressure) + ", " + 
    String(heightTP) + ", " + 
    //String(heightAcc) + ", " + 
    String(upwardsAcc) + ", " +
    String(mpu6050.getAccY()) + ", " +
    String(mpu6050.getAccZ()) + ", " +
    String(mpu6050.getGyroX()) + ", " +
    String(mpu6050.getGyroY()) + ", " +
    String(mpu6050.getGyroZ()) + ", " +
    String(mpu6050.getAccAngleX()) + ", " +
    String(mpu6050.getAccAngleY()) + ", " +
    String(mpu6050.getGyroAngleX()) + ", " +
    String(mpu6050.getGyroAngleY()) + ", " +
    String(mpu6050.getGyroAngleZ()) + ", " +
    String(mpu6050.getAngleX()) + ", " +
    String(mpu6050.getAngleY()) + ", " +
    String(mpu6050.getAngleZ());
    print_data(data);
    Serial.println(data);
    Serial.println("test");
  }
  String msg = "Wrote sensor data to file";
  print_log(msg);
  Serial.println("nuts");

  return data;
}

// Average Pressure at sea level
float const P0 = 1013.25;

double calcHeightTP(double temp, double pressure) {
    return ((pow((pressure / P0), (1/5.257)) - 1) * (temp + 273.15) / 0.0065);
}

// set status-LED based on state of flight
void set_led(State state ) {
  switch (state) {
    case State::Ready:
      analogWrite(3, 0);
      analogWrite(5, 255);
      analogWrite(6, 0);
      break;
    case State::Error:
      analogWrite(3, 0);
      analogWrite(5, 0);
      analogWrite(6, 255);
      break;
  }
}

void setup_led() {
  pinMode(6, OUTPUT); // red
  pinMode(5, OUTPUT); // green
  pinMode(3, OUTPUT); // blue
}

void setup_logging() {
  setup_sd();
}

// connect to SD and create File-objects
void setup_sd() {
  const String DATA_FILE = "test_data.csv";
  const String LOG_FILE = "test_log.txt";
  const int SD_PORT = 10;

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PORT)) {
    Serial.println("SD initialization failed!");
    STATE = State::Error;
  }
  print_log("SD initialization done.");

  data_file = SD.open(DATA_FILE, FILE_WRITE);
  log_file = SD.open(LOG_FILE, FILE_WRITE);
}

void setup_sensors() {
    Serial.print("MS5611 ");
    Serial.println(MS5611.begin() ? "found" : "not found");

    Serial.print("calibrating MPU6050...");
    mpu6050.calcGyroOffsets(true);
    Serial.println("Done");
    
    String header = "Time, TempMPU, TempMS, Pressure, heightTP, heightAcc, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, AccAngleX, AccAngleY, GyroAngleX, GyroAngleY, GyroZ, AngleX, AngleY, AngleZ";
    print_data(header);
}
