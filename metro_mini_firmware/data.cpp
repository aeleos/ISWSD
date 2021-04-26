#include "data.h"
#include <SPI.h>
#include <SD.h>


void Dataset::write_data(float time, bool button, float pressure) {
  Serial.print(time,4);
  Serial.print(",");
  Serial.print(button);
  Serial.print(",");
  Serial.println(pressure,6);

  File myFile = SD.open(filename, FILE_WRITE);
  myFile.print(time,4);
  myFile.print(",");
  myFile.print(button);
  myFile.print(",");
  myFile.println(pressure,6);
  Serial.println(filename);

  myFile.close();

}
