#ifndef LCD_h
#define LCD_h

#include "LiquidCrystal_I2C.h"

//  LCD Functions
class LCD : public LiquidCrystal_I2C
{
  public:
    LCD(uint8_t addr, uint8_t cols, uint8_t rows);
    void setup(); // setup the lcd conditions
    void setTopStatusIndiciators(bool card, uint8_t sat); // print the battery percentage in the top right corner
    void setTopStatusText(const char* status_text);
    void setTopStatusNumber(uint8_t num_zero, uint8_t num_measurements);
    void progress_loop(uint8_t col, uint8_t row, int loops); // print a spinning wheel, updates after loops calls
    void startup_screen(); // top row for lcd startup message
    void gpslock_screen(); // top row for gps search screen
    void zero_prompt_screen(char * preset = NULL); // top row for when no 0s are set
    void standard_screen(uint8_t zero, uint8_t meas, char * preset = NULL); // top row for during measurement taking
    void zero_max(uint8_t meas);
    void print_measurement(float x, float y, float z, float w, bool is_zero, char * preset = NULL); // print out the measurements taken
    void take_measurement();
    void confirm_measurement();
    void input_measure_zero();
    void input_zero();
    bool preset_select();

    uint8_t loopcount = 0;
    char loop = '\\';

};




#endif
