#include "data.h"
#include "Print.h"

Dataset::Dataset(uint8_t zero){
  Dataset::name_file(zero);
}

void Dataset::name_file(uint8_t zero){
  int i = zero % 10;
  filename[2] = i+30;
  (zero>99) ? filename[1] = int((zero-i)/10) : filename[1] = 0;
  filename[3] = '.';
  filename[4] = 'c';
  filename[5] = 's';
  filename[6] = 'v';
  filename[7] = '\0';
  filename[0] = 'F';
  return;
}

void Dataset::set_zero(float x, float y, float pressure){
  zero_pressure = pressure;
  x_coordinate[0] = x;
  y_coordinate[0] = y;
  return;
}

int Dataset::get_zero_pressure(void){
  return zero_pressure;
}

void Dataset::record_measurement(float x, float y, float meas){
  x_coordinate[dp] = x;
  y_coordinate[dp] = y;
  measurements[dp] = meas * 3.28084;
  dp++;
  return;
}
