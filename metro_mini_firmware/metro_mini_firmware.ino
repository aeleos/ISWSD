
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
//#include "data.h"
#include "pinout.h"
//#include <SD.h>


// Pressure Sensor Object
Adafruit_DPS310 dps;


// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600
// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(7, 8);

// GPS Object
TinyGPS gps;

float dps_zero = 0.0;
float gps_lat_zero, gps_lon_zero, gps_alt_zero = 0.0;

void setup()
{

  pinMode(YES_PIN, INPUT_PULLUP);
  pinMode(NO_PIN, INPUT_PULLUP);
  pinMode(SS_PIN, OUTPUT);
  pinMode(CD_PIN, INPUT);

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);


  lcd.setup();                      // initialize the lcd
  lcd.startup_screen();


  gps_ss.begin(GPS_BAUD);

  // Initialize Adafruit DPS310

  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    ;
  }

  // Setup highest precision
  dps.setMode(DPS310_CONT_PRESSURE);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);


  Serial.println(F("Init. Done"));

}

enum global_state {
  NO_GPS_LOCK,
  NO_ZERO_SET,
  READY_FOR_LOCATION,
  CONFIRM_SAVE_LOCATION,
};

global_state current_state = NO_GPS_LOCK;
global_state last_state = CONFIRM_SAVE_LOCATION;

bool last_yes_button = true;
bool last_no_button = true;
bool last_card_inserted = false;

uint8_t num_zero = 0;
uint8_t num_measurements = 0;


void loop()
{

  // read all data inputs

  // gps
  float gps_lat, gps_lon, gps_alt;
  uint8_t gps_sats = TinyGPS::GPS_INVALID_SATELLITES;
  long unsigned int age;

  gps_sats = gps.satellites();
  gps_alt = gps.f_altitude();
  gps.f_get_position(&gps_lat, &gps_lon, &age);

  // DPS310 measurements
  float dps_altitude = dps.readAltitude();

  // digital inputs
  bool yes_button = !digitalRead(YES_PIN);
  bool no_button = !digitalRead(NO_PIN);
  bool card_inserted = digitalRead(CD_PIN);

  bool yes_button_changed = yes_button != last_yes_button;
  bool no_button_changed = no_button != last_no_button;
  bool card_inserted_changed = card_inserted != last_card_inserted;

  last_yes_button = yes_button;
  no_button_changed = last_no_button;
  card_inserted_changed = last_card_inserted;

  // is this the first time we are on this state
  bool has_state_changed = false;

  if (current_state != last_state) {
    last_state = current_state;
    has_state_changed = true;
  }



  // do current state actions
  switch (current_state) {
    case NO_GPS_LOCK:
      {
        // update lcd
        if (has_state_changed) {
          lcd.gpslock_screen();
        }

        Serial.println("HERE NO_GPS_LOCK");

        break;
      }
    case NO_ZERO_SET:
      {

        if (has_state_changed) {
          lcd.zero_prompt_screen();
          num_zero++;
          num_measurements = 0;
        }
        Serial.println("HERE NO_ZERO_SET");

        break;
      }
    case READY_FOR_LOCATION:
      {

        if (has_state_changed) {
          num_measurements++;
          lcd.standard_screen(num_zero, num_measurements);
        }
        Serial.println("HERE READY_FOR_LOCATION");

        break;
      }
    case CONFIRM_SAVE_LOCATION:
      {

        if (has_state_changed) {
          lcd.print_measurement(num_zero, num_measurements, gps_lat, gps_lon, dps_altitude);
        }
        Serial.println("HERE CONFIRM_SAVE_LOCATION");


        break;
      }


  }




  // update current state if needed


  // do current state actions
  bool should_transition = false;

  bool was_yes_pressed = yes_button && yes_button_changed;
  bool was_no_pressed = no_button && no_button_changed;


  switch (current_state) {
    case NO_GPS_LOCK:
      {

        should_transition = was_yes_pressed || gps_sats != TinyGPS::GPS_INVALID_SATELLITES;

        if (should_transition)
          current_state = NO_ZERO_SET;

        break;
      }
    case NO_ZERO_SET:
      {

        should_transition = was_yes_pressed;

        if (should_transition)
          current_state = READY_FOR_LOCATION;

        break;
      }
    case READY_FOR_LOCATION:
      {
        should_transition = was_yes_pressed || was_no_pressed;

        if (should_transition) {
          if (was_yes_pressed) {
            current_state = CONFIRM_SAVE_LOCATION;
          }

          if (was_no_pressed)
            current_state = NO_ZERO_SET;
        }

        break;
      }
    case CONFIRM_SAVE_LOCATION:
      {

        should_transition = was_yes_pressed;

        if (should_transition)
          current_state = READY_FOR_LOCATION;

        break;
      }


  }


  // wait

  if (has_state_changed) {
    lcd.top_bar(card_inserted, gps_sats);
  }

  lcd.progress_loop(11, 0, 1);

  delay_and_read_gps(1000);
}


static void delay_and_read_gps(unsigned long ms) {
  unsigned long start = millis();
  do
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}
