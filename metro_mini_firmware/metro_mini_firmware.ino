
/*
   Metro Mini Pinout

    D0, D1 are USB Serial Debugging

    D2 through D12 are digital IO,
    D3, D5, D6, D9, D10, D11 PWM

    Our Pin Assignments

    D2 is Measure/Yes/Accept, D4 is Zero/No
    D6 Low Battery Warning, connected to LBO pin, will be pulled to ground if battery is low (can potentionally combine this with A3 if LBO is actually connected to BAT
    D7, D8 Software Serial for GPS
    D9 CD SD Card Detect
    D10 CS, D11 MOSI, D12 MISO, D13 Clock, SPI for SD Card
    A3 Battery Voltage, connected to BAT pin on PowerBoost
    A4 Data, A5 Clock I2C for LCD, Altimeter

    sudo chmod 777 /dev/ttyUSB0
*/





//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Adafruit_DPS310.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "lcd.h"
#include <inttypes.h>
#include "data.h"
#include "pinout.h"
#include <SD.h>

//void freeRam ()
//{
//  extern int __heap_start, *__brkval;
//  int v;
//  Serial.print(F("Free RAM: "));
//  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
//  return;
//}

void record_measurement(float * h, long * lat,long * lon,unsigned long * d, unsigned long * t, Adafruit_DPS310 * dps, TinyGPS * gps){
  sensors_event_t temp_event, pressure_event;

  while (!dps->temperatureAvailable() || !dps->pressureAvailable()) {
    continue; // wait until there's something to read
  }

  dps->getEvents(&temp_event, &pressure_event);
  gps->get_position(lat, lon);
  gps->get_datetime(d, t);


  return;
}

// Pressure Sensor Object
Adafruit_DPS310 dps;
//Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
//Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600
// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(7, 8);

// GPS Object
TinyGPS gps;

Dataset* data;

struct state_indicators {
  unsigned int card_available : 1;
  unsigned int custom_exists : 1;
  unsigned int custom_chosen : 1;
  unsigned int zero_set : 1;
  unsigned int measurement_count : 6;
  unsigned int gps_override : 1;
  unsigned int top_bar_update: 1;
  unsigned int last_yes_pushed : 1;
  unsigned int last_no_pushed : 1;
  uint8_t zero_count;
};

float zero_hPa;

uint8_t state = 0;

typedef union {
    struct {
      int gps_lock : 1;
      int datapoint_limit : 1;
      int zeropoint_limit : 1;
      int standard : 1;
    };
    uint8_t bit_clear;
} lcd_state;

lcd_state lcds;

struct state_indicators si;

bool yes_pushed,no_pushed;

void setup()
{

  pinMode(YES_PIN, INPUT_PULLUP);
  pinMode(NO_PIN, INPUT_PULLUP);
  pinMode(SS_PIN, OUTPUT);
  pinMode(CD_PIN, INPUT);

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Initialize the LCD, will call Wire.begin()
  lcds.bit_clear = 0;

  lcd.setup();                      // initialize the lcd
  lcd.startup_screen();

  // Card detect
  si.zero_set = 0;
  si.measurement_count = 0;
  si.card_available = (bool)digitalRead(CD_PIN);
  si.gps_override = 0;
  si.zero_count = 1;

  if (! si.card_available) {
    delete data;
  }
  else {
    data = new Dataset;
    if (SD.begin(SS_PIN)) {
      si.custom_exists = data->get_files(&si.zero_count);
      Serial.print(F("Card with file count "));
      Serial.println(si.zero_count);
    }
    else {
      delete data;
      Serial.println(F("Card init failed."));
      si.card_available = 0;
    }
  }

  gps_ss.begin(GPS_BAUD);

  // Initialize Adafruit DPS310
  
  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    //Serial.print(F("... "));
    ;
  }

  // Setup highest precision
  dps.setMode(DPS310_CONT_PRESSURE);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);

  //Serial.println(F("Done"));

  //Serial.println(F("Sensor Details"));

  //dps_temp->printSensorDetails();
  //dps_pressure->printSensorDetails();

  Serial.println(F("Init. Done"));

}


void loop()
{
  
  delay_and_read_gps(300);
  uint8_t num_sats = gps.satellites();
  float h;
  long lat, lon;
  unsigned long d = TinyGPS::GPS_INVALID_DATE, t = TinyGPS::GPS_INVALID_TIME;
  
  
  switch (state) {
    case 0:  // no GPS lock
      {
        
        delay_and_read_gps(1000);
        
        if (! lcds.gps_lock ) {
          lcds.bit_clear = 0;
          lcds.gps_lock = 1;
          lcd.gpslock_screen();
          si.top_bar_update = 1;
        }
        lcd.progress_loop(11, 0, 1);
        
        si.gps_override = (yes_pushed || si.gps_override);
        
        delay_and_read_gps(500);
        if (si.card_available) {
          data->name_file(si.custom_chosen, si.zero_count);
        }
        
        break;
      }
      
    case 1: // unset zero or too many datapoints
      {
        
        if (si.custom_exists) {
          lcd.clear();
          si.custom_chosen = lcd.custom_select();
        }
        
        if (si.custom_chosen) {
          data->get_custom_location();
          lcd.zero_prompt_screen(data->custom_name);
        } 
        else {
            lcd.zero_prompt_screen();
          }
          
        if (no_pushed) {
          state = 3;
        }
        
        if (state != 3) {
          break;
        }
      }
      
    case 2: // too many zeros
      {
        
        if (!si.custom_chosen) {
          si.zero_count = 0;
          break;
        }
        
        if (! lcds.zeropoint_limit) {
          lcd.zero_max(si.measurement_count);
          lcds.zeropoint_limit = 0;
          lcds.zeropoint_limit = 1;
        }
        
        si.custom_chosen = 0;
        si.card_available = 0;
        si.top_bar_update = 1;
        break;
      }
      
    case 3: // press for zero
      {
               
        lcd.clear();
        si.top_bar_update = 1;
        
        if (si.zero_set == 0) {
          si.zero_set = 1;
        } 
        else {
          si.zero_count++;
        }
        
        if (si.custom_exists) {
          si.custom_chosen = lcd.custom_select();
          if (si.custom_chosen) {
            data->get_custom_location();
            lcd.zero_prompt_screen(data->custom_name);
          } 
        }
        else {
          lcd.zero_prompt_screen();
        }
        
        if (si.card_available) {
          data->reset();
          data->name_file(si.custom_chosen, si.zero_count);
        }


        sensors_event_t temp_event, pressure_event;

        while (!dps.temperatureAvailable() || !dps.pressureAvailable()) {
          continue; // wait until there's something to read
        }

        if (si.card_available) {
          data->record_measurement(lat, lon, 0.0, d, t);
        }

        if (si.custom_chosen) {
          data->get_custom_location();
          lcd.zero_prompt_screen(data->custom_name);
        } else {
          lcd.print_zero(si.zero_count, lat, lon);
        }
        lcds.bit_clear = 0;
        //Serial.println(F("Exit zero"));
        break;
      }
    case 4: // press for measurement
      {

        lcd.clear();
        si.top_bar_update = 1;

        data->record_measurement(lat, lon, h, d, t);

        if (si.custom_chosen) {
          data->get_custom_location();
          lcd.print_measurement(si.zero_count, si.measurement_count, lat, lon, h, data->custom_name);
        } else {
          lcd.print_measurement(si.zero_count, si.measurement_count, lat, lon, h);
        }
        lcds.bit_clear = 0;
        si.measurement_count++;
        //Serial.println(F("Exit meas"));
        break;
      }
    case 5:
      {
        
        if (!lcds.standard) {
          lcd.standard_screen(si.zero_count, si.measurement_count);
          si.top_bar_update = 1;
          lcds.bit_clear = 0;
          lcds.standard = 1;
        }
      }
      break;
  }

  lcd.top_bar(si.card_available, num_sats, si.top_bar_update);
  si.top_bar_update = 0;
  
  bool yes_pushed = ((!digitalRead(YES_PIN)) && (!si.last_yes_pushed));
  bool no_pushed = ((!digitalRead(NO_PIN)) && (!si.last_yes_pushed));
  si.last_yes_pushed = yes_pushed;
  si.last_no_pushed = no_pushed;


  // state determination
  if ( (num_sats == TinyGPS::GPS_INVALID_SATELLITES) && (!si.gps_override)) // no GPS lock
  { 
    state = 0;
  }
  else if ((!si.zero_set) || (si.measurement_count > 50)) // zero needs to be set
  { 
    state = 1;
  }
  else if (si.zero_count > 99) // too many zeros
  { 
    state = 2;
  }
  else if (no_pushed) // command to zero is input
  { 
    state = 3;
  }
  else if (yes_pushed) // command to measure is input
  { 
    state = 4;
  }
  else { // ready to take measurement
    state = 5;
  }
}


static void delay_and_read_gps(unsigned long ms) {
  unsigned long start = millis();
  do
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}
