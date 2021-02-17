#include "data.h"

Dataset::Dataset(uint8_t type, uint8_t zero){
  Dataset::zero = zero;
  Dataset::type = type;
}

void Dataset::setup(){
  Dataset::filename = String(Dataset::zero) + '.csv'
}
