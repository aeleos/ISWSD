#include "data.h"
#include <SPI.h>
#include <SD.h> 

void Dataset::name_file(bool custom){
  int i = file_number % 10;
  filename[2] = i+30;
  (file_number>99) ? filename[1] = int((file_number-i)/10) : filename[1] = 0;
  (custom) ? filename[0] = 'C' : filename[0] = 'F';
  return;
}

void Dataset::reset(){
  read_to_character = 0;
  return;
}

void Dataset::set_zero(long x, long y, float pressure, unsigned long d, unsigned long t){
  zero_hPa = pressure;
  record_measurement(x,y,0.0,d,t);
  return;
}

float Dataset::get_zero_pressure(void){
  return zero_hPa;
}

void Dataset::record_measurement(long x, long y, float meas, unsigned long d, unsigned long t){
    File myFile = open(filename, FILE_WRITE);
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
