#ifndef LCD_h
#define LCD_h

#include "LiquidCrystal_I2C.h"

//  LCD Functions 
class LCD : public LiquidCrystal_I2C
{
  public:
    LCD(uint8_t addr, uint8_t cols, uint8_t rows);
    void setup(); // setup the lcd conditions
    void top_bar(bool card,uint8_t sat,bool change); // print the battery percentage in the top right corner
    void progress_loop(uint8_t col, uint8_t row, int loops); // print a spinning wheel, updates after loops calls
    void startup_screen(); // top row for lcd startup message
    void gpslock_screen(); // top row for gps search screen
    void zero_prompt_screen(char * custom = NULL); // top row for when no 0s are set
    void standard_screen(uint8_t zero, uint8_t meas, char * custom = NULL); // top row for during measurement taking
    void zero_max(uint8_t meas);
    void print_measurement(uint8_t zero, uint8_t meas, float x, float y, float z,char * custom = NULL); // print out the measurements taken
    void print_zero(uint8_t zero, float x, float y,char * custom = NULL); // print out the location of the zero
    bool custom_select();
    void wait_for_press(int pin);
  private:
    uint8_t loopcount = 0;
    char loop = '\\';
    uint8_t old_percent;
    uint8_t old_sat;
};




#endif
