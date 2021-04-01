#include <SPI.h>
#include <SD.h>

const int SD_CS_PORT = PA4;

// global FILE-objects for SD access
File LOG_FILE;
File DATA_FILE;
