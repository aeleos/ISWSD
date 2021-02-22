#include "data.h"
#include "Print.h"

void Dataset::name_file(uint8_t zero, bool custom){
  int i = zero % 10;
  filename[2] = i+30;
  (zero>99) ? filename[1] = int((zero-i)/10) : filename[1] = 0;
  (custom) ? filename[0] = 'C' : filename[0] = 'F';
  return;
}

void Dataset::set_zero(float x, float y, float pressure){
  zero_pressure = pressure;
  x_coordinate[0] = x;
  y_coordinate[0] = y;
  dp = 1;
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
