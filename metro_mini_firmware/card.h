#ifndef CARD_h
#define CARD_h

#include <SPI.h>
#include <SD.h>
#include <inttypes.h>
#include "data.h"

//  SD card functions
class Card : public SDClass
{
  public:
    bool get_files();
    uint8_t get_custom_locations(); // returns the number of custom locations input
    char custom_names[50][13]; // names set by custom
    uint8_t file_number = 0;
    void log_data(Dataset data);
};




#endif 
