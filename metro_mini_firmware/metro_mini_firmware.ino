
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
//#include <SimpleKalmanFilter.h>
#include "FilterButterworth.h">

//#include "Kalman.h"
//using namespace BLA;
//
//#define Nstate 1 // vertical position
//#define Nobs 2   // barometer position, gps position
//
//BLA::Matrix<Nobs> obs; // observation vector
//KALMAN<Nstate, Nobs> K; // your Kalman filter

// Pressure Sensor Object
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();
Adafruit_Sensor *dps_temperature = dps.getTemperatureSensor();


// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600
// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(7, 8);

// GPS Object
TinyGPS gps;


void lat_to_ft(float lat) {

}

struct MeasSet {
  float gps_lat = 0;
  float gps_lon = 0;
  float gps_alt = 0;
  float dps_alt = 0;
};


Dataset* data;



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

  // SD Card
  data = new Dataset;
  if (SD.begin(SS_PIN)) {

  }

  //  K.F = {1.0};
  //
  //  K.H = {
  //    1.0,
  //    1.0
  //  };
  //
  //  K.R = {
  //    1,
  //    0.05
  //  };
  //
  //  K.Q = {1};

}

enum global_state {
  NO_GPS_LOCK,
  READY_FOR_ZERO_SET,
  CONFIRM_ZERO_SET,
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

uint8_t num_loops = 0;


//SimpleKalmanFilter altitude_kf(0.001, 0.01, 0.01);

struct MeasSet recent_meas;
struct MeasSet saved_meas;
struct MeasSet zero_meas;

FilterBuBs2* filter = new FilterBuBs2();

uint8_t gps_sats = TinyGPS::GPS_INVALID_SATELLITES;
long unsigned int age;


float getSeaHpaFromAlt(float P, float h, float T) {

  return P * pow((1 - (0.0065 * h) / (T + 0.0065 * h + 273.15)), -5.257);
}

float last_temp = 0;

float sea_level_hpa = 0;

float last_alt = 0;


void loop()
{

  // read all data inputs

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

  float current_alt;
  float alt_estimate;
  
  if (current_state > CONFIRM_ZERO_SET) {
    last_alt = current_alt;
    current_alt = dps.readAltitude(sea_level_hpa);
//    alt_estimate = altitude_kf.updateEstimate(current_alt);
    alt_estimate = filter->step(current_alt-last_alt);
    //    obs = {
    //      current_alt,
    //      recent_meas.gps_alt
    //    };
    //    K.update(obs);
    record_measurement(alt_estimate, alt_estimate, alt_estimate);
    recent_meas.dps_alt = alt_estimate;
  }



  // do current state actions
  switch (current_state) {
    case NO_GPS_LOCK:
      {
        // update lcd
        if (has_state_changed) {
          lcd.gpslock_screen();
        }

        break;
      }
    case READY_FOR_ZERO_SET:
      {

        if (has_state_changed) {
          lcd.zero_prompt_screen();
        }

        break;
      }
    case CONFIRM_ZERO_SET:
      {

        if (has_state_changed) {
          lcd.print_measurement(num_zero, num_measurements, recent_meas.gps_lat, recent_meas.gps_lon, recent_meas.dps_alt, recent_meas.gps_alt);
          saved_meas = recent_meas;

        }

        break;
      }
    case READY_FOR_LOCATION:
      {

        if (has_state_changed) {
          lcd.standard_screen(num_zero, num_measurements);
          lcd.input_measure_zero();
        }

        break;
      }
    case CONFIRM_SAVE_LOCATION:
      {

        //        if (has_state_changed) {
        if (num_loops % 10 == 0) {
          lcd.print_measurement(num_zero, num_measurements, zero_meas.gps_lat - recent_meas.gps_lat, zero_meas.gps_lon - recent_meas.gps_lon, recent_meas.dps_alt, recent_meas.gps_alt);
          saved_meas = recent_meas;

        }



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
          current_state = READY_FOR_ZERO_SET;

        break;
      }
    case READY_FOR_ZERO_SET:
      {

        should_transition = was_yes_pressed;

        if (should_transition)
          current_state = CONFIRM_ZERO_SET;

        break;
      }
    case CONFIRM_ZERO_SET:
      {

        should_transition = was_yes_pressed || was_no_pressed;

        if (should_transition) {

          if (was_yes_pressed) {

            zero_meas = saved_meas;

            sea_level_hpa = getSeaHpaFromAlt(zero_meas.dps_alt, zero_meas.gps_alt, last_temp);

            num_zero++;
            num_measurements = 0;

            data->record_measurement('F', zero_meas.gps_lat, zero_meas.gps_lon, zero_meas.dps_alt, zero_meas.gps_alt, 0);

            current_state = READY_FOR_LOCATION;
          }

          if (was_no_pressed) {
            current_state = READY_FOR_ZERO_SET;
          }
        }

        break;
      }
    case READY_FOR_LOCATION:
      {
        should_transition = was_yes_pressed || was_no_pressed;

        if (should_transition) {
          if (was_yes_pressed) {
            current_state = CONFIRM_SAVE_LOCATION;
            num_measurements++;
            data->record_measurement('M', zero_meas.gps_lat - saved_meas.gps_lat, zero_meas.gps_lon - saved_meas.gps_lon, saved_meas.dps_alt, saved_meas.gps_alt, 0);
          }


          if (was_no_pressed)
            current_state = READY_FOR_ZERO_SET;
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

  if (has_state_changed || (num_loops % 20 == 0)) {
    lcd.top_bar(card_inserted, gps_sats);
  }


  if (num_loops % 10 == 0)
    lcd.progress_loop(11, 0, 1);

  delay_and_read_gps(100);
  num_loops++;
}


void record_measurement(float val1, float val2, float val3) {
  Serial.print("\t");

  Serial.print(val1);

  Serial.print(" ");
  Serial.print(val2);

  Serial.print(" ");
  Serial.println(val3);
}


static void delay_and_read_gps(unsigned long ms) {
  unsigned long start = millis();
  sensors_event_t pressure_event, temperature_event;
  do
  {
    // Do state estimation updates

    if (current_state == READY_FOR_ZERO_SET) {

      if (dps.temperatureAvailable()) {
        dps_temperature->getEvent(&temperature_event);
        last_temp = temperature_event.pressure;
      }

      if (dps.pressureAvailable()) {
        dps_pressure->getEvent(&pressure_event);
        recent_meas.dps_alt = pressure_event.pressure;
      }
    }



    while (gps_ss.available()) {

      gps.encode(gps_ss.read());

      gps_sats = gps.satellites();
      recent_meas.gps_alt = gps.f_altitude();
      gps.f_get_position(&recent_meas.gps_lat, &recent_meas.gps_lon, &age);


    }
  } while (millis() - start < ms);
}
