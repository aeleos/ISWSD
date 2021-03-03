#include "LiquidCrystal_I2C.h"
#include "lcd.h"
#include <Arduino.h>
#include "pinout.h"
// LCD Functions

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

const PROGMEM uint16_t MAX_VOLT = 880;
const PROGMEM uint16_t  MIN_VOLT = 574;
const PROGMEM float SCALE_VOLT = 0.3267973856209;


uint8_t voltage_to_percent(){
  uint16_t volt = (uint16_t)analogRead(BAT_PIN);
  uint8_t percent;
  if (volt > (pgm_read_byte(&MAX_VOLT))){
    percent = 100;
  }
  else{
    percent = (uint8_t)(volt-(pgm_read_byte(&MIN_VOLT)) * (pgm_read_byte(&SCALE_VOLT)));
  }
  return percent;
}

const PROGMEM uint8_t delta[] = {
  0x04,
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x11,
  0x1F,
  0x00
};

const PROGMEM uint8_t backslash[] = {
  0x00,
  0x10,
  0x08,
  0x04,
  0x02,
  0x01,
  0x00,
  0x00

};

LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows) : LiquidCrystal_I2C(addr,cols,rows)
{
  
}

void LCD::setup(){
  LCD::init();
  LCD::backlight();
  LCD::createChar(0, delta);
  LCD::createChar(1, backslash);
  LCD::noCursor();
  LCD::noBlink();
  return;
}

void LCD::top_bar(bool card){
  uint8_t percent = voltage_to_percent();
  LCD::setCursor(15,0);
  if (card) LCD::print(F("C"));
  LCD::setCursor(17,0); 
  LCD::print(percent);
  if (percent < 100){
  LCD::print(F("%"));
  }
  return;
}

void LCD::progress_loop(uint8_t col, uint8_t row, int loops){
  if (loopcount < loops-1){
    loopcount++;
    return;
  }
  if (loopcount >= loops){
    loopcount=0;
  }
  else{
  LCD::setCursor(col,row);
  if (loop == '/') {
    LCD::print('-');
    loop = '-';
  }
  else if (loop == '-') {
    LCD::printByte(1);
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
  LCD::print(F("System startup"));
  return;
}

bool LCD::gpslock_screen(int sats,int invalid_sats){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(F("GPS Search"));
  LCD::setCursor(0, 0);
  LCD::print(F("Satellites: "));
  if (sats == invalid_sats) {
    LCD::print(F("No Lock"));
  } else {
    LCD::print(sats);
  }
  return (bool)!digitalRead(YES_PIN);
}

void LCD::zero_prompt_screen(char * custom){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(F("Hold to zero."));
  if (custom)
  {
  LCD::setCursor(0, 1);
  LCD::print(F("Zero point: "));
  LCD::print(*custom);
  }
  return;
}

void LCD::standard_screen(uint8_t zero, uint8_t meas, char * custom){
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(zero);
  LCD::print(F(":"));
  if (custom)
  {
  LCD::print(*custom);
  }
  else{
  LCD::print(meas);
  }
  return;
}

void LCD::zero_max(uint8_t meas){
  LCD::standard_screen(99,meas);
  LCD::setCursor(0, 1);
  LCD::print(F("SD card full."));
  LCD::setCursor(0, 2);
  LCD::print(F("Data not stored."));
  LCD::setCursor(0, 3);
  LCD::print(F("Press to continue."));
  while (digitalRead(YES_PIN)){;} 
  delay(PIN_DB);
  return;
}


void LCD::datapoint_max(uint8_t zero){
  LCD::standard_screen(zero,49);
  LCD::setCursor(0, 1);
  LCD::print(F("Data point limit."));
  LCD::setCursor(0, 2);
  LCD::print(F("Hold to zero."));
  return;
}

void LCD::print_measurement(uint8_t zero, uint8_t meas, float x, float y, float z,char * custom){
  LCD::standard_screen(zero,meas);
  LCD::setCursor(0, 1);
  LCD::print(F("Point "));
  LCD::print(zero);
  LCD::print(F(":"));
  if (custom){
    LCD::setCursor(0, 3);
    LCD::print(custom);
  }
  else{
   LCD::print(meas-1); 
  }
  LCD::setCursor(0, 2);
  LCD::printByte(0);
  LCD::print(F("h: "));
  LCD::print(z);
  LCD::print(F("ft"));
  LCD::setCursor(0, 3);
  LCD::print(F("At "));
  LCD::print(x);
  LCD::print(F(","));
  LCD::print(y); 
  while (digitalRead(YES_PIN)){;} 
  delay(PIN_DB);
  return;
}

void LCD::print_zero(uint8_t zero, float x, float y,char * custom){
  LCD::standard_screen(zero,0);
  LCD::setCursor(0, 1);
  LCD::print(F("Set zero point "));
  LCD::print(zero);
  LCD::setCursor(0, 2);
  LCD::print(F("Location: "));
  LCD::print(x);
  LCD::print(F(","));
  LCD::print(y);  
  if (custom){
    LCD::setCursor(0, 3);
    LCD::print(custom);
  }
  while (digitalRead(YES_PIN)){;}
  delay(PIN_DB);
  return;
}

bool LCD::custom_select(){
  LCD::setCursor(0, 3);
  LCD::print(F("Use preset locations?"));
  while (1){
    if (!digitalRead(YES_PIN)){
      delay(PIN_DB);
      return 1;}
    else if (!digitalRead(NO_PIN)){
      delay(PIN_DB);
      return 0;}
  }
  
}
