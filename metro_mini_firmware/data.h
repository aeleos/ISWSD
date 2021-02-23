#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>

class Dataset : public SDClass
{
  public:
    void name_file(uint8_t zero, bool custom);
    void set_zero(float x, float y, float pressure, uint16_t year, uint8_t month, uint8_t day);
    float get_zero_pressure(void);
    void record_measurement(float x, float y, float meas, uint16_t year, uint8_t month, uint8_t day);
    char filename[8] = "XXX.csv"; // name to which to write, based on which zero number this is
    // SD functions
    bool get_files();
    uint8_t get_custom_locations(); // returns the number of custom locations input
    char custom_names[50][13]; // names set by custom
    uint8_t file_number = 0;
  private:
    float zero_hPa; // pressure in hPa at the zero point
    
};

#endif
