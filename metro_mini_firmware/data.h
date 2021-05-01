#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

class Dataset
{
  public:
    char filename[9]; // name to which to write, based on which zero number this is
    void write_data(float time, bool button, float pressure);
    void init();
    // SD functions


};

#endif
