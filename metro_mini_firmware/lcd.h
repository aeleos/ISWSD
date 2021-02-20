#ifndef LCD_h
#define LCD_h

#include "LiquidCrystal_I2C.h"

//  LCD Functions 
class LCD : private LiquidCrystal_I2C
{
  public:
    LCD(uint8_t addr, uint8_t cols, uint8_t rows);
    void setup(); // setup the lcd conditions
    void top_bar(uint8_t percent,bool card); // print the battery percentage in the top right corner
    void progress_loop(uint8_t col, uint8_t row, int loops); // print a spinning wheel, updates after loops calls
    void startup_screen(); // top row for lcd startup message
    void gpslock_screen(int sats,int invalid_sats); // top row for gps search screen
    void zero_prompt_screen(char * custom); // top row for when no 0s are set
    void standard_screen(uint8_t zero, uint8_t meas, char * custom); // top row for during measurement taking
    void zero_max(uint8_t meas);
    void datapoiont_max(uint8_t zero);
    void print_measurement(uint8_t zero, uint8_t meas, float x, float y, float z); // print out the measurements taken
    void print_zero(uint8_t zero, float x, float y); // print out the location of the zero
  private:
    int loopcount = 0;
    char loop = '\\';
};




#endif
