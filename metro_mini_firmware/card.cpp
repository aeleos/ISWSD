#include "card.h"
#include <SPI.h>
#include <SD.h> 
#include <inttypes.h>
#include "data.h"

bool Card::get_files(){
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
      get_custom_locations();
      }
    entry.close();
  }
}

uint8_t Card::get_custom_locations(File custom){
  char ch;
  uint8_t r = 0,c = 0;
  
  custom = open("cust.txt");
  while (custom.available()) {
    ch = custom.read();
    while (ch != '\n'){
        custom_names[r][c] = ch;
        c++;
        ch = custom.read();
    }
    c = 0;
    r++;
    }
  custom.close();
  return r+1;
}

void Card::log_data(Dataset data){
  File myFile = open(data.filename, FILE_WRITE);

  
  myFile.close();
  return;
}
