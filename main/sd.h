#include <SPI.h>
#include <SD.h>

const int SD_CS_PORT = PA4;

// global FILE-objects for SD access
File LOG_FILE;
File DATA_FILE;

// connect to SD and create File-objects
void setup_sd() {
  String data_file = "-data.csv";
  String log_file = "-log.txt";
  // TODO: Is this pin correct?

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("SD initialization failed!");
    set_led(255, 0, 0);
    while (true) {}
  }
  Serial.println("SD initialization done.");

  File number_file = SD.open("fileidx.txt", FILE_WRITE);
  int pos = number_file.position();
  number_file.write('e');
  number_file.close();

  data_file = pos + data_file;
  log_file = pos + log_file;

  DATA_FILE = SD.open(data_file, FILE_WRITE);
  LOG_FILE = SD.open(log_file, FILE_WRITE);

  Serial.println(data_file + " " + log_file);
}

// print one logging statement to logfile and serial
void print_log(String && msg) {
  // print to serial monitor
  Serial.println(msg);

  // print to file
  LOG_FILE.print(millis());
  LOG_FILE.print(": ");
  LOG_FILE.println(msg);
  LOG_FILE.flush();
}

/* MACROS */
#define PRINT_VALUE(value)                       \
  Serial.print(value); Serial.print(",");        \
  DATA_FILE.print(value); DATA_FILE.print(",");

#define PRINTLN_VALUE(value)                     \
  Serial.println(value);                         \
  DATA_FILE.println(value);                      \
  DATA_FILE.flush();
