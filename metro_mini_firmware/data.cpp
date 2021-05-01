#include "data.h"
#include <SPI.h>
#include <SD.h>

void Dataset::init() {
  File root = SD.open("/");

  int file_tracker = 1;
  while (true) {
    sprintf(filename, "%d.csv", file_tracker);
    if (SD.exists(filename)) {
      file_tracker++;
    } else {
      break;
    }
  }

}

void Dataset::write_data(float time, bool button, float pressure) {

  File myFile = SD.open(filename, FILE_WRITE);
  myFile.print(time,2);
  myFile.print(",");
  myFile.print(button);
  myFile.print(",");
  myFile.println(pressure,6);
  Serial.println(filename);

  myFile.close();

}
