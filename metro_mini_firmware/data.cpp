#include "data.h"
#include <SPI.h>
#include <SD.h> 

void Dataset::name_file(uint8_t zero, bool custom){
  int i = zero % 10;
  filename[2] = i+30;
  (zero>99) ? filename[1] = int((zero-i)/10) : filename[1] = 0;
  (custom) ? filename[0] = 'C' : filename[0] = 'F';
  return;
}

void Dataset::reset(){
  read_to_character = 0;
  return;
}

void Dataset::set_zero(float x, float y, float pressure,  uint16_t year, uint8_t month, uint8_t day){
  zero_hPa = pressure;
  record_measurement(x,y,0.0,year,month,day);
  return;
}

float Dataset::get_zero_pressure(void){
  return zero_hPa;
}

void Dataset::record_measurement(float x, float y, float meas, uint16_t year, uint8_t month, uint8_t day){
    File myFile = open(filename, FILE_WRITE);
    myFile.print(year);
    myFile.print('-');
    myFile.print(month);
    myFile.print('-');
    myFile.print(day);
    myFile.print(',');
    myFile.print(x);
    myFile.print(',');
    myFile.print(y);
    myFile.print(',');
    myFile.println(meas);
  myFile.close();
  return;
}

bool Dataset::get_files(){
  File dir;
  dir = open("/");
  uint8_t c=0;
  bool custom = 0;
  
    while (true) {
    file_number++;
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return custom;
    }
    dir = entry;
    
    if (entry.name() == "cust.txt") {
      custom = 1;
      file_number--;
      }
    entry.close();
  }
}

uint8_t Dataset::get_custom_location(){
  char ch;
  uint8_t c = 0;
  uint16_t place = 0; 
  bool av = 0;

  File custom;
  custom = open(F("cust.txt"));
  while (custom.available() && place < read_to_character) {
    ch = custom.read();
  }
  ch = custom.read();
  while (custom.available() && ch != '\n'){
        av = 1;
        custom_name[c] = ch;
        c++;
        place++;
        ch = custom.read();
    }
  read_to_character = place+1;
  custom_name[c] = '\0';
  custom.close();
  return av;
}
