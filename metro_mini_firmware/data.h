#ifndef DATA_h
#define DATA_h

#include <inttypes.h>

//  Dataset for non-custom use
class Dataset
{
  public:
    Dataset(); //start a new dataset object after every zero
    void setup(uint8_t zero);
    void set_zero(float x, float y, float pressure);
    int get_zero_pressure(void);
    void record_measurement(float x, float y, float meas);
    void save_data();
  private:
    uint8_t dp = 1; // datapoint we are on
    int zero_pressure; // pressure in hPa at the zero point
    char filename[7]; // name to which to write, based on which zero number this is
    float x_coordinate[50], y_coordinate[50]; // array of gps coordinates
    float measurements[50] = {0}; // elevation change
    
};

class DatasetC : public Dataset
{
  public:
    DatasetC();
    char custom_names[50][13]; // names set by custom     
};

#endif
