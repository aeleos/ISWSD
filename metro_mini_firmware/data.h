#ifndef DATA_h
#define DATA_h

#include <inttypes.h>
#include <SPI.h>
#include <SD.h>

struct date { 
    unsigned int d : 5; 
    unsigned int m : 4; 
    unsigned int y; 
}; 

class Dataset : public SDClass
{
  public:
    void reset();
    void name_file(uint8_t zero, bool custom);
    void set_zero(float x, float y, float pressure, struct date dt);
    float get_zero_pressure(void);
    void record_measurement(float x, float y, float meas, struct date dt);
    char filename[8] = "XXX.csv"; // name to which to write, based on which zero number this is
    // SD functions
    bool get_files();
    uint8_t get_custom_location(); // returns the number of custom locations input
    char custom_name[13]; // name set by custom
    uint8_t file_number = 0;
  private:
    float zero_hPa; // pressure in hPa at the zero point
    uint16_t read_to_character = 0;
    
};

#endif
