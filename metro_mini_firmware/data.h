#ifndef DATA_h
#define DATA_h

#include <inttypes.h>

struct datetime {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} ;

//  Dataset for non-custom use
class Dataset
{
  public:
    void name_file(uint8_t zero, bool custom);
    void set_zero(float x, float y, float pressure);
    int get_zero_pressure(void);
    void record_measurement(float x, float y, float meas);
    char filename[8] = "XXX.csv"; // name to which to write, based on which zero number this is
    uint8_t dp = 1; // datapoint we are on
    float x_coordinate[50], y_coordinate[50]; // array of gps coordinates
    float measurements[50] = {0}; // elevation change
    datetime moments[50];
  private:
    int zero_pressure; // pressure in hPa at the zero point
    
};

#endif
