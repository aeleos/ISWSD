#include "data.h"
#include "Print.h"

Dataset::Dataset(){
}

void Dataset::setup(uint8_t zero){
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

DatasetC::DatasetC(){

}
