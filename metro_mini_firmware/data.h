#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

class Dataset
{
  public:
    Dataset() {
      File root = SD.open("/");

      int file_tracker = 1;
      while (true) {
        sprintf(filename, "%d.csv", file_tracker);
        if (SD.exists(filename)) {
          file_tracker++;
        } else {
          break;
        }
      }


    }
    char filename[9]; // name to which to write, based on which zero number this is
    void write_data(float time, bool button, float pressure);
    // SD functions


};

#endif
