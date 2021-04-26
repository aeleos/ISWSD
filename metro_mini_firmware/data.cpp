#include "data.h"
#include <SPI.h>
#include <SD.h>

//void Dataset::name_file(bool preset, uint8_t file_number) {
//  int i = file_number % 10;
//  filename[2] = i + 30;
//  filename[1] = int((file_number - i) / 10) + 30;
//  (preset) ? filename[0] = 'C' : filename[0] = 'F';
//  return;
//}

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

float Dataset::get_zero_pressure(void) {
  return zero_hPa;
}

void Dataset::record_measurement(char c, float x, float y, float meas, float d, unsigned long t) {
  File myFile = SD.open(filepath, FILE_WRITE);
  myFile.print(c);
  myFile.print(',');
  myFile.print(d,8);
  myFile.print(',');
  myFile.print(t);
  myFile.print(',');
  myFile.print(x,8);
  myFile.print(',');
  myFile.print(y,8);
  myFile.print(',');
  myFile.println(meas,8);
  myFile.close();
  return;
}

void Dataset::test() {
  File myFile = SD.open(filepath, FILE_WRITE);
  for (int i = 0; i < 10; i++) {
      myFile.print("here");
  }
  myFile.close();
  return;
}

bool Dataset::check_preset() {
  File dir;
  Serial.println(F("Opening Directory... "));
  dir = SD.open("/");
  bool preset = 0;

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return preset;
    }
    dir = entry;
    if (entry.name() == "preset.txt") {
      preset = 1;
    }
    entry.close();
  }
}

uint8_t Dataset::get_preset_location() {
  char ch;
  uint8_t c = 0;
  uint16_t place = 0;
  bool av = 0; // is a preset location available

  File preset;
  preset = SD.open(F("cust.txt"));
  while (preset.available() && place < read_to_character) {
    ch = preset.read();
  }
  ch = preset.read();
  while (preset.available() && ch != '\n') {
    av = 1;
    preset_name[c] = ch;
    c++;
    place++;
    ch = preset.read();
  }
  read_to_character = place + 1;
  preset_name[c] = '\0';
  preset.close();
  return av;
}
