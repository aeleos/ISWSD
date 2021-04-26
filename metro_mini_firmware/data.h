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
      File root = SD.open("/data");

      int max_file = 0;
      char buf[20];
      while (true) {
        File entry = root.openNextFile();
        if (!entry) {
          break;
        }
        int current = atoi(strtok(entry.name(), "."));
        max_file = max(current, max_file);
      }

      sprintf(filepath, "/data/%d.csv", max_file+1);
      sprintf(filename, "%d.csv", max_file+1);

    }
    void reset();
    void test();
    void set_zero(long x, long y, unsigned long d, unsigned long t);
    float get_zero_pressure(void);
    void record_measurement(char c, float x, float y, float meas, float d, unsigned long t);
    char filename[9] = "data.csv"; // name to which to write, based on which zero number this is
    char filepath[20];
    // SD functions
    bool check_preset();
    uint8_t get_preset_location(); // returns the number of preset locations input
    char preset_name[13]; // name set by preset
  private:
    float zero_hPa; // pressure in hPa at the zero point
    uint16_t read_to_character = 0;

    //Sd2Card card;
    //SdVolume volume;

};

#endif
