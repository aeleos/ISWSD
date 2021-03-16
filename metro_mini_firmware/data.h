#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>


class Dataset
{
  public:
    void reset();
    void set_zero(long x, long y, unsigned long d, unsigned long t);
    float get_zero_pressure(void);
    void record_measurement(char c, float x, float y, float meas, float d, unsigned long t);
    char filename[9] = "data.csv"; // name to which to write, based on which zero number this is
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
