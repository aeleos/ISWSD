#include "LiquidCrystal_I2C.h"
#include "lcd.h"
// LCD Functions

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t delta[] = {
  0x04,
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x11,
  0x11,
  0x1F
};

LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows) : LiquidCrystal_I2C(addr,cols,rows)
{
  
}

void LCD::setup(){
  LCD::init();
  LCD::backlight();
  LCD::createChar(0, delta);
  LCD::noCursor();
  LCD::noBlink();
  return;
}

void LCD::print_battery(int percent){
  LCD::setCursor(17,0); 
  LCD::print(percent);
  return;
}

void LCD::progress_loop(uint8_t col, uint8_t row, int loops){
  if (loopcount < loops){
    loopcount++;
    return;
  }
  else{
  LCD::setCursor(col,row);
  if (loop == '/') {
    LCD::print('-');
    loop = '-';
  }
  else if (loop == '-') {
    LCD::print('\\');
    loop = '\\';
  }
  else if (loop == '\\') {
    LCD::print('|');
    loop = '|';
  }
  else{
    LCD::print('/');
    loop = '/';
  }
  loopcount = 0;
  return;
  }
}

void LCD::startup_screen(){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print("System startup");
  return;
}

void LCD::gpslock_screen(int sats){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print("Satellites: ");
  LCD::print(sats);
  return;
}

void LCD::zero_prompt_screen(){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print("Hold to zero.");
  return;
}

void LCD::standard_screen(int zero, int meas){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(zero);
  LCD::print(':');
  LCD::print(meas);
  return;
}

void LCD::print_measurement(int zero, int meas, float x, float y, float z){
  LCD::standard_screen(zero,meas+1);
  LCD::setCursor(0, 1);
  LCD::print("Point ");
  LCD::print(zero);
  LCD::print(':');
  LCD::print(meas);
  LCD::setCursor(0, 2);
  LCD::printByte(0);
  LCD::print("h: ");
  LCD::print(z);
  LCD::setCursor(0, 3);
  LCD::print("At: ");
  LCD::print(x);
  LCD::print(',');
  LCD::print(y);  
  return;
}

void LCD::print_zero(int zero, float x, float y){
  LCD::standard_screen(zero,0);
  LCD::setCursor(0, 1);
  LCD::print("Set zero point ");
  LCD::print(zero);
  LCD::setCursor(0, 2);
  LCD::print("Location: ");
  LCD::print(x);
  LCD::print(',');
  LCD::print(y);  
  return;
}
