#include "data.h"
#include <SPI.h>
#include <SD.h>

void Dataset::name_file(bool custom, uint8_t file_number) {
  int i = file_number % 10;
  filename[2] = i + 30;
  filename[1] = int((file_number - i) / 10) + 30;
  (custom) ? filename[0] = 'C' : filename[0] = 'F';
  return;
}

//bool Dataset::Data_init(){
////  if (!card.init(SPI_HALF_SPEED, SS_PIN)) { return 0;}
////  if (!volume.init(card)) { return 0;}
//  if (!SD.begin(10)) {return 0;}
//    return 1;
//}

void Dataset::reset() {
  read_to_character = 0;
  return;
}


void Dataset::set_zero(long x, long y, unsigned long d, unsigned long t) {
//  record_measurement(x, y, 0.0, d, t);
  return;
}

float Dataset::get_zero_pressure(void) {
  return zero_hPa;
}

void Dataset::record_measurement(char* file, long x, long y, float meas, unsigned long d, unsigned long t) {
  File myFile = SD.open(file, FILE_WRITE);
  myFile.print(d);
  myFile.print(',');
  myFile.print(t);
  myFile.print(',');
  myFile.print(x);
  myFile.print(',');
  myFile.print(y);
  myFile.print(',');
  myFile.println(meas);
  myFile.close();
  return;
}

bool Dataset::get_files(uint8_t * file_count) {
  File dir;
  Serial.println(F("Opening Directory... "));
  dir = SD.open("/");
  bool custom = 0;

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return custom;
    }
    dir = entry;
    *file_count++;
    Serial.print(F("File found: "));
    Serial.println(*file_count);

    if (entry.name() == "cust.txt") {
      custom = 1;
      *file_count--;
    }
    entry.close();
  }
}

uint8_t Dataset::get_custom_location() {
  char ch;
  uint8_t c = 0;
  uint16_t place = 0;
  bool av = 0;

  File custom;
  custom = SD.open(F("cust.txt"));
  while (custom.available() && place < read_to_character) {
    ch = custom.read();
  }
  ch = custom.read();
  while (custom.available() && ch != '\n') {
    av = 1;
    custom_name[c] = ch;
    c++;
    place++;
    ch = custom.read();
  }
  read_to_character = place + 1;
  custom_name[c] = '\0';
  custom.close();
  return av;
}
