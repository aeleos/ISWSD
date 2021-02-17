#ifndef DATA_h
#define DATA_h

#include <inttypes.h>

//  LCD Functions 
class Dataset
{
  public:
    Dataset(uint8_t type, uint8_t zero); //start a new dataset object after every zero
    void setup();
    void set_zero(float x, float y, float pressure);
    int get_zero_pressure(void);
    void record_measurement(float x, float y, float meas);
  private:
    uint8_t dp = 1; // datapoints taken
    uint8_t type,zero; // for constructor
    int zero_pressure; // pressure in hPa at the zero point
    char filename[20]; // name to which to write, based on which zero number this is
    float x_coordinate[50], y_coordinate[50]; // array of gps coordinates
    float measurements[50] = {0}; // elevation change
    
};

#endif
