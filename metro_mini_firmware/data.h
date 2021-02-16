#ifndef DATA_h
#define DATA_h

#include <inttypes.h>

//  LCD Functions 
class Dataset
{
  public:
    dataset(int type); //start a new dataset object after every zero
    void save(); // write the data to the SD card before erasing every zero
  private:
    char filename[7]; // name to which to write, based on which zero number this is
    float x_coordinate[50], y_coordinate[50]; // array of gps coordinates
    uint16_t measurements[500] = {0}; // start 0 at 1, basically add 1 to every measurement
};

#endif
