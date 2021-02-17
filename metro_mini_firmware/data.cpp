#include "data.h"

Dataset::Dataset(uint8_t type, uint8_t zero){
  Dataset::zero = zero;
  Dataset::type = type;
}

void Dataset::setup(){
  Dataset::filename = String(Dataset::zero) + '.csv';
  
  return;
}

void set_zero(float x, float y, float pressure){
  Dataset::zero_pressure = pressure;
  Dataset::x_coordinate[0] = x;
  Dataset::y_coordinate[0] = y;
  return;
}

float get_zero_pressure(void){
  return Dataset::zero_pressure;
}

void record_measurement(float x, float y, float meas){
  Dataset::x_coordinate[dp] = x;
  Dataset::y_coordinate[dp] = y;
  Dataset::measurements[dp] = meas * 3.28084;
  dp++;
  return;
}
