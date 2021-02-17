#ifndef DATA_h
#define DATA_h

#include <inttypes.h>

//  LCD Functions 
class Dataset
{
  public:
    Dataset(uint8_t type, uint8_t zero); //start a new dataset object after every zero
    void setup();
    void save(); // write the data to the SD card before erasing every zero
  private:
    uint8_t type,zero;
    int zero_pressure;
    char filename[7]; // name to which to write, based on which zero number this is
    float x_coordinate[50], y_coordinate[50]; // array of gps coordinates
    uint16_t measurements[50] = {0}; // elevation change
    
};

#endif
