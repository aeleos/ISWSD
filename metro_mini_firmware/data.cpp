#include "data.h"
#include "Print.h"

Dataset::Dataset(uint8_t type, uint8_t zero){
  Dataset::zero = zero;
  Dataset::type = type;
}

void Dataset::setup(){
  int i = zero % 10;
  filename[1] = i+30;
  (zero>99) ? filename[0] = int((zero-i)/10) : filename[0] = 0;
  filename[2] = '.';
  filename[3] = 'c';
  filename[4] = 's';
  filename[5] = 'v';
  filename[6] = '\0';
  return;
}

void Dataset::set_zero(float x, float y, float pressure){
  Dataset::zero_pressure = pressure;
  Dataset::x_coordinate[0] = x;
  Dataset::y_coordinate[0] = y;
  return;
}

int Dataset::get_zero_pressure(void){
  return Dataset::zero_pressure;
}

void Dataset::record_measurement(float x, float y, float meas){
  Dataset::x_coordinate[dp] = x;
  Dataset::y_coordinate[dp] = y;
  Dataset::measurements[dp] = meas * 3.28084;
  dp++;
  return;
}
