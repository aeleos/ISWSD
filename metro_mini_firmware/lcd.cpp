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

//const PROGMEM uint16_t MAX_VOLT = 860;
//const PROGMEM uint16_t MID_VOLT = 778;
//const PROGMEM uint16_t MIN_VOLT = 756;
//const PROGMEM uint8_t OFFSET_VOLTH = 50;
//const PROGMEM uint8_t OFFSET_VOLTL = 21;
//const PROGMEM float SCALE_VOLTH = 0.60975609756;
//const PROGMEM float SCALE_VOLTL = 1.27272727273;
#define MAX_VOLT 860
#define MID_VOLT 778
#define MIN_VOLT 756
#define OFFSET_VOLTH 50
#define OFFSET_VOLTL 21
#define SCALE_VOLTH 0.60975609756
#define SCALE_VOLTL 1.27272727273

#define LEFT_BUTTON_CURSOR 0
#define RIGHT_BUTTON_CURSOR 12


uint8_t voltage_to_percent() {
  uint16_t volt = (uint16_t)analogRead(BAT_PIN);
  //Serial.print(F("Battery voltage: "));
  //Serial.print(volt);
  uint8_t percent;
  if (volt >= MAX_VOLT) {
    percent = 100;
  }
  else if (volt < MIN_VOLT) {
    percent = 0;
  }
  else if (volt >= MID_VOLT) {
    percent = (uint8_t)(((volt - MID_VOLT) * SCALE_VOLTH) + OFFSET_VOLTH);
  }
  else {
    percent = (uint8_t)(((volt - MIN_VOLT) * SCALE_VOLTL) + OFFSET_VOLTL);
  }
  //Serial.print(F(" - "));
  //Serial.println(percent);
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

LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows) : LiquidCrystal_I2C(addr, cols, rows)
{


}


void LCD::setup() {
  LCD::init();
  LCD::backlight();
  LCD::createChar(0, delta);
  LCD::createChar(1, backslash);
  LCD::noCursor();
  LCD::noBlink();
  return;
}

void LCD::top_bar(bool card, uint8_t sat) {
  uint8_t percent = voltage_to_percent();

    LCD::setCursor(14, 0);
    if (sat > 9 && sat < 0xFF) {
      LCD::print(9);
    }
    else if (sat < 9) {
      LCD::print(sat);
    }
    LCD::setCursor(15, 0);
    if (card) LCD::print(F("C"));
    LCD::setCursor(17, 0);
    if (percent == 0 ) {
      LCD::print(F("LOW"));
    }
    else {
      LCD::print(percent);
      if (percent < 100) {
        LCD::print(F("%"));
      }
    }
  
  return;
}

void LCD::progress_loop(uint8_t col, uint8_t row, int loops) {
  if (loopcount < loops - 1) {
    loopcount++;
    return;
  }
  if (loopcount >= loops) {
    loopcount = 0;
  }
  else {
    LCD::setCursor(col, row);
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
    else {
      LCD::print('/');
      loop = '/';
    }
    loopcount = 0;
    return;
  }
}

void LCD::startup_screen() {
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(F("System startup"));
  return;
}

void LCD::gpslock_screen() {
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(F("GPS Search"));
  LCD::setCursor(0, 1);
  LCD::print(F("Satellites: "));
  LCD::print(F("No Lock"));
  LCD::input_acknowledge();
  return;
}

void LCD::zero_prompt_screen(char * preset) {
  LCD::clear();
  LCD::setCursor(0,0);
  LCD::print(F("Set Origin"));
  LCD::setCursor(0, 1);
  LCD::print(F("Hold to set."));
  if (preset)
  {
    LCD::setCursor(0, 2);
    LCD::print(F("Zero point: "));
    LCD::print(*preset);
  }
  LCD::input_zero();
  return;
}

void LCD::standard_screen(uint8_t zero, uint8_t meas, char * preset) {
  LCD::clear();
  LCD::setCursor(0, 0);
  LCD::print(zero);
  LCD::print(F(":"));
  if (preset)
  {
    LCD::print(*preset);
  }
  else {
    LCD::print(meas);
  }
  return;
}

void LCD::zero_max(uint8_t meas) {
  LCD::standard_screen(99, meas);
  LCD::setCursor(0, 1);
  LCD::print(F("SD card full."));
  LCD::setCursor(0, 2);
  LCD::print(F("Data not stored."));
  LCD::input_acknowledge();
  return;
}

void LCD::print_measurement(uint8_t zero, uint8_t meas, float x, float y, float z, char * preset) {
  LCD::standard_screen(zero, meas, preset);
  LCD::setCursor(0, 1);
  LCD::printByte(0);
  LCD::print(F("h: "));
  LCD::print(z);
  LCD::print(F("ft"));
  LCD::setCursor(0, 2);
  LCD::print(long(x));
  LCD::print(F(","));
  LCD::print(long(y));
  LCD::input_acknowledge();
  return;
}

//void LCD::print_zero(uint8_t zero, float x, float y, char * preset) {
//  LCD::standard_screen(zero, 0, preset);
//  LCD::setCursor(0, 1);
//  LCD::print(F("Origin"));
//  LCD::setCursor(0, 2);
//  LCD::print(long(x));
//  LCD::print(F(","));
//  LCD::print(long(y));
//  LCD::input_acknowledge();
//  return;
//}

void LCD::input_acknowledge(){
  LCD::setCursor(LEFT_BUTTON_CURSOR,3);
  LCD::print(F("Accept"));
  return;
}

void LCD::input_zero(){
  LCD::setCursor(RIGHT_BUTTON_CURSOR,3);
  LCD::print(F("Zero"));
  return;
}

void LCD::input_yes_no(){
  LCD::setCursor(LEFT_BUTTON_CURSOR,3);
  LCD::print(F("Yes"));
  LCD::setCursor(RIGHT_BUTTON_CURSOR,3);
  LCD::print(F("No"));
  return;
}

void LCD::input_measure_zero(){
  LCD::setCursor(LEFT_BUTTON_CURSOR,3);
  LCD::print(F("Measure"));
  LCD::setCursor(RIGHT_BUTTON_CURSOR,3);
  LCD::print(F("Zero"));
  return;
}

bool LCD::preset_select() {
  LCD::setCursor(0, 2);
  LCD::print(F("Use presets?"));
  LCD::input_yes_no();
  return;
}
