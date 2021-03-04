
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
//#include <Wire.h>
#include <Adafruit_DPS310.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "lcd.h"
#include <inttypes.h>
#include "data.h"
#include "pinout.h"

void freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  Serial.print(F("Free RAM: "));
  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
  return;
}

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

struct state_indicators {
  unsigned int card : 1;
  unsigned int customE : 1;
  unsigned int custom : 1;
  unsigned int zero : 1;
  unsigned int meas : 6;
  unsigned int gps_override : 1;
  unsigned int lcd_clear: 1;
  uint8_t zero_count;
};

float zero_hPa;

uint8_t state = 0;

char lcd_state; // startup screen, lcd si from MSB to LSB: Setup screen, GPSLock screen, standard screen, zero prompt, measurement print, zero print, meas max, zero max,

struct state_indicators si;

void setup()
{

  pinMode(ME_PIN, INPUT_PULLUP);
  pinMode(ZE_PIN, INPUT_PULLUP);
  //pinMode(CD_PIN, INPUT_PULLUP);

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);

  //Serial.println(F("USB SI started..."));

  //Serial.print(F("LCD init... "));

  // Initialize the LCD, will call Wire.begin()

  lcd.setup();                      // initialize the lcd
  lcd.startup_screen();
  lcd_state = 0b10000000;

  //Serial.println(F("Done"));

  // Card detect
  si.zero = 0;
  si.meas = 0;
  si.card = (bool)digitalRead(CD_PIN);
  si.gps_override = 0;
  si.zero_count = 0-1;
  Serial.print(si.zero_count);

  if (! si.card){
    delete data;
    Serial.println(F("No card, destructing dataclass"));
  }
  else{
    data = new Dataset;
    si.customE = data->get_files(&si.zero_count);
    Serial.print(F("Card with file count "));
    Serial.println(si.zero_count);
  }

  // Initialize GPS Software Serial
  //Serial.print(F("GPS init... "));
  gps_ss.begin(GPS_BAUD);

  // Initialize Adafruit DPS310

  //Serial.print(F("DPS310 init... "));
  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    //Serial.print(F("... "));
    ;
  }
  //Serial.print(F("Config... "));

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  //Serial.println(F("Done"));

  //Serial.println(F("Sensor Details"));

  //dps_temp->printSensorDetails();
  //dps_pressure->printSensorDetails();

  Serial.println(F("Init. Done"));

}


void loop()
{
 // execution  
 switch(state){
  case 0:  // no GPS lock
  {
    //Serial.println(F("Enter GPS Lock"));
    delay_and_read_gps(1000);
    if (lcd_state != 0b01000000) {lcd_state = 0b01000000; lcd.gpslock_screen();}
    si.gps_override = (!(bool)digitalRead(YES_PIN) || si.gps_override);
    lcd.progress_loop(11,0,1);
    if (si.card) {data->name_file(si.custom,si.zero_count);}
    //Serial.println(F("Exit GPS Lock"));
    break;
  }
  case 1: // unset zero
  {
    //Serial.println(F("Enter Zero Prompt"));
    if (lcd_state != 0b00010000){
      if (si.customE) {lcd.clear(); si.custom = lcd.custom_select();}
      if(si.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else { lcd.zero_prompt_screen(); }
      lcd_state = 0b00010000;
      si.lcd_clear = 1;
    }
    if (!(bool)digitalRead(ZE_PIN)){
        //Serial.println(F("HERE!"));
        si.zero = 1;
        state = 4;
      }
      //Serial.println(F("Exit zero prompt"));
    if (state != 4) {break;}
  }
  case 2: // too many data points
  {
    if ( lcd_state != 0b00100010){
      lcd.datapoint_max(si.zero_count);
      lcd_state = 0b00100010; }
    break;
  }
  case 3: // too many zeros
  {
    if (!si.custom){
      si.zero_count = 0;
      break;
    }
    if ( lcd_state != 0b00100001){
      lcd.zero_max(si.meas);
      lcd_state = 0b00100001;
    }
    si.custom = 0;
    si.card = 0;
    lcd.card_overwrite();
    break;
  }
  case 4: // press for zero
  {
    //Serial.println(F("Enter zero"));
    if (si.card){  data->reset(); }
    lcd.clear();
    si.lcd_clear = 1;
    //Serial.print(F("Read GPS... "));
    delay_and_read_gps(500);
    //Serial.print(F("Data collected... "));
    si.zero_count++;    
    if (si.customE) {si.custom = lcd.custom_select();}
    if(si.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else { lcd.zero_prompt_screen(); }
    if (si.card) {data->name_file(si.custom,si.zero_count);}
    //Serial.println(F("Data manip ..."));
    //freeRam();
    

    sensors_event_t temp_event, pressure_event;
  
    while (!dps.temperatureAvailable() || !dps.pressureAvailable()) {
      return; // wait until there's something to read
    }

      dps.getEvents(&temp_event, &pressure_event);
      float h = pressure_event.pressure;
   

    //float h = dps.readAltitude(data->get_zero_pressure());
    long lat, lon;
    unsigned long d = TinyGPS::GPS_INVALID_DATE, t = TinyGPS::GPS_INVALID_TIME;
    if (! si.gps_override){
      gps.get_position(&lat,&lon);
      gps.get_datetime(&d,&t);
    }

    Serial.println(h);
      
    if (si.card){data->set_zero(lat,lon,d,t);}

    if(si.custom){ data->get_custom_location(); lcd.zero_prompt_screen(data->custom_name);} else {lcd.print_zero(si.zero_count,lat,lon);}
    lcd_state=0b00000000;
    Serial.println(F("Exit zero"));
    break;
  }
  case 5: // press for measurement
  {
    Serial.println(F("Enter meas"));
    lcd.clear();
    si.lcd_clear = 1;
    delay_and_read_gps(500);

    
    float h = dps.readAltitude(zero_hPa);
    long lat = 0, lon = 0;
    unsigned long d= TinyGPS::GPS_INVALID_DATE, t = TinyGPS::GPS_INVALID_TIME;
    if (! si.gps_override){
      gps.get_position(&lat,&lon);
      gps.get_datetime(&d,&t);
    }
      
      if(si.card){data->record_measurement(lat,lon,h,d,t);}

    if(si.custom){ data->get_custom_location(); lcd.print_measurement(si.zero_count,si.meas,lat,lon,h,data->custom_name);} else { lcd.print_measurement(si.zero_count,si.meas,lat,lon,h);}
    lcd_state=0b00000000;
    si.meas++;
    Serial.println(F("Exit meas"));
    break;
  }
  default: 
  {
    if (lcd_state != 0b00100000){
    lcd.standard_screen(si.zero_count,si.meas);
    si.lcd_clear = 1;
    lcd_state = 0b00100000;}
  }
    break;
 }

   uint8_t num_sats = gps.satellites();

lcd.top_bar(si.card,num_sats,si.lcd_clear);
si.lcd_clear = 0;


// state determination
  if ( (num_sats == TinyGPS::GPS_INVALID_SATELLITES) && (!si.gps_override)){  // no GPS lock
    //Serial.println(num_sats);
    state = 0;
    
  }
  else if (!si.zero){ // zero is not set
    state = 1;
  }
  else if (si.meas > 50){  // too many datapoints
     state = 2;
    }
  else if (si.zero_count > 99){  // too many zeros
    state = 3;
  }
  else if (! (bool)digitalRead(ZE_PIN)){ // if command to zero is input
    state = 4;
  }
  else if (! (bool)digitalRead(ME_PIN)){ // command to measure is input
    state = 5;
  }
  else {state = 6;}

}


static void delay_and_read_gps(unsigned long ms){
  unsigned long start = millis();
  do 
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}
