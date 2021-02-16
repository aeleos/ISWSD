#include "LiquidCrystal_I2C.h"
#include "lcd.h"
// LCD Functions

LCD::LCD(int addr, int cols, int rows) : LiquidCrystal_I2C(addr,cols,rows)
{
  
}

void LCD::setup(){
  LCD::init();
  LCD::backlight();
  LCD::noCursor();
  LCD::noBlink();
  return;
}

void LCD::print_battery(int percent){
  LCD::setCursor(17,0); 
  LCD::print(percent);
  return;
}

void LCD::progress_loop(int col, int row, int loops){
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
  String output = "GPS: " + String(sats) + " sats";
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(output);
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
  LCD::print("Measurement ");
  LCD::print(zero);
  LCD::print(':');
  LCD::print(meas);
  LCD::setCursor(0, 2);
  LCD::print("Relative elevation change: ");
  LCD::print(z);
  LCD::setCursor(0, 3);
  LCD::print("Location: ");
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