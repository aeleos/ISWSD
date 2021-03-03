#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>


class Dataset : public SDClass
{
  public:
    void reset();
    void name_file(bool custom,uint8_t file_number);
    void set_zero(long x, long y, float pressure, unsigned long d, unsigned long t);
    float get_zero_pressure(void);
    void record_measurement(long x, long y, float meas, unsigned long d, unsigned long t);
    char filename[8] = "XXX.csv"; // name to which to write, based on which zero number this is
    // SD functions
    bool get_files(uint8_t * file_count);
    uint8_t get_custom_location(); // returns the number of custom locations input
    char custom_name[13]; // name set by custom
  private:
    float zero_hPa; // pressure in hPa at the zero point
    uint16_t read_to_character = 0;
    
};

#endif
