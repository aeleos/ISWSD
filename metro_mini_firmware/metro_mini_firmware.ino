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
#include <Wire.h>
#include <Adafruit_DPS310.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "lcd.h"
#include <inttypes.h>
#include "data.h"
#include "pinout.h"

// Pressure Sensor Object
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600
// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(7, 8);

// GPS Object
TinyGPS gps;

Dataset* data;

struct state_machine {
  unsigned int card : 1;
  unsigned int customE : 1;
  unsigned int custom : 1;
  unsigned int zero : 1;
  unsigned int meas : 6;
  unsigned int gps_override : 1;
};

char lcd_state; // startup screen, lcd state from MSB to LSB: Setup screen, GPSLock screen, standard screen, zero prompt, measurement print, zero print, meas max, zero max,

struct state_machine state;

void setup()
{

  pinMode(ME_PIN, INPUT_PULLUP);
  pinMode(ZE_PIN, INPUT_PULLUP);

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("USB SI started..."));

  Serial.print(F("LCD init... "));

  // Initialize the LCD, will call Wire.begin()

  lcd.setup();                      // initialize the lcd
  lcd.startup_screen();
  lcd_state = 0b10000000;

  Serial.println(F("Done"));

  // Card detect
  state.zero = 0;
  state.meas = 0;
  state.card = digitalRead(CD_PIN);
  state.gps_override = 0;

  if (! state.card){
    delete data;
    Serial.println(F("No card, destructing dataclass"));
  }
  else{
    data = new Dataset;
    state.customE = data->get_files();
    Serial.print(F("Card with file count "));
    Serial.println(data->file_number);
  }

  // Initialize GPS Software Serial
  Serial.print(F("GPS init... "));
  gps_ss.begin(GPS_BAUD);

//  while(1){
//  uint8_t num_sats;
//
//  num_sats = gps.satellites();
//  unsigned long age, date, time, chars = 0;
//  short unsigned int sentences = 0, failed = 0;
//
//  gps.stats(&chars, &sentences, &failed);
//
//
//
//  Serial.println(chars);
//  Serial.println(sentences);
//  Serial.println(failed);
//
//  lcd.gpslock_screen(num_sats, TinyGPS::GPS_INVALID_SATELLITES);
//  lcd.top_bar(state.card);
//  lcd_state = 0b01000000;
//  
//  if (num_sats == TinyGPS::GPS_INVALID_SATELLITES) {
//    Serial.println(F("GPS has no lock"));
//  } else {
//    Serial.print(F("GPS: "));
//    Serial.print(num_sats);
//    Serial.println(F(" Sats"));
//  }
//  Serial.println(F("Done"));
//  }

  // Initialize Adafruit DPS310

  Serial.print(F("DPS310 init... "));
  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    Serial.print(F("... "));
  }
  Serial.print(F("Config... "));

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  Serial.println(F("Done"));

  Serial.println(F("Sensor Details"));

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();

  Serial.println(F("Init. Done"));

}


void loop()
{
  uint8_t num_sats;

  num_sats = gps.satellites();
  unsigned long age, date, time, chars = 0;
  short unsigned int sentences = 0, failed = 0;

  gps.stats(&chars, &sentences, &failed);



  Serial.println(chars);
  Serial.println(sentences);
  Serial.println(failed);


  ///// check GPS stuff above 
  if ( 1 ){  // no GPS lock
    state.gps_override = lcd.gpslock_screen(num_sats, TinyGPS::GPS_INVALID_SATELLITES) || state.gps_override;
    
  }
  else if (!state.zero){ // zero is not set
    if (lcd_state != 0b00010000){
      if (state.customE) {lcd.clear(); state.custom = lcd.custom_select();}
      if(state.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else { lcd.zero_prompt_screen(); }
      lcd_state = 0b00010000;
      state.zero = 1;
    }
  }
  else if (state.meas > 50){  // too many datapoints
    if ( lcd_state != 0b00100010){
      lcd.datapoint_max(data->file_number);
      lcd_state = 0b00100010; }
    }
  else if (data->file_number > 100 && state.custom){  // too many zeros
    if ( lcd_state != 0b00100001){
      lcd.zero_max(state.meas);
      lcd_state = 0b00100001;
    }
    state.custom = 0;
    state.card = 0;
  }
  else if (! digitalRead(ZE_PIN)){ // if command to zero is input
    data->reset();
    lcd.clear();
    delay(PIN_DB);
    if (state.customE) {state.custom = lcd.custom_select();}
    if(state.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else { lcd.zero_prompt_screen(); }
    data->file_number++;    
    float lat = 100.0,lon=100.0;
    

    if(state.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else { lcd.zero_prompt_screen(); }lcd.print_zero(data->file_number,lat,lon);
    lcd_state=0b00000000;
  }
  else if (! digitalRead(ME_PIN)){ // command to meaasure is input
    lcd.clear();
    delay(PIN_DB);
    float lat = 100.0,lon=100.0,h;

    if(state.custom){ data->get_custom_location(); lcd.print_measurement(data->file_number,state.meas,lat,lon,h,data->custom_name);} else { lcd.print_measurement(data->file_number,state.meas,lat,lon,h);}
    lcd_state=0b00000000;
  }
  else if (!lcd_state == 0b00000000){ // reset to standard screen
    if(state.custom){lcd.standard_screen(data->file_number,state.meas,data->custom_name);} else { lcd.standard_screen(data->file_number,state.meas); }
  }


  lcd.top_bar(state.card);

}


//static void delay_and_read_gps(unsigned long ms){
//  unsigned long start = millis();
//  do 
//  {
//    while (gps_ss.available())
//      gps.encode(gps_ss.read());
//  } while (millis() - start < ms);
//}
