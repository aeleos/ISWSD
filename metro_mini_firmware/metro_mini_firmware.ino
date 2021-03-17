
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


/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   INCLUDES
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/

#include <Adafruit_DPS310.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "lcd.h"
#include <inttypes.h>
#include "data.h"
#include "pinout.h"
#include <SD.h>
#include <SimpleKalmanFilter.h>
#include "FilterButterworth.h">

/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   OBJECTS AND VARIABLES
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/



/*
   ------------------------------------------------------------------
   DPS 310 Pressure Sensor
   ------------------------------------------------------------------
*/

Adafruit_DPS310 dps;
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();
Adafruit_Sensor *dps_temperature = dps.getTemperatureSensor();

sensor_event_t sensor_event;

// https://keisan.casio.com/exec/system/1224575267
float getSeaLevelPressureFromAlt(float pressure, float height, float temperature) {
  return (pressure) * pow((1 - (0.0065 * height) / (temperature + 0.0065 * height + 273.15)), -5.257);
}

// https://github.com/adafruit/Adafruit_DPS310/blob/master/Adafruit_DPS310.cpp
// https://keisan.casio.com/exec/system/1224585971
float getAltFromSeaLevelPressure(float seaLevelPressure, float currentPressure, float temperature) {
  // return 44330 * (1.0 - pow((currentPressure / 100) / seaLevelPressure, 0.1903));
  return ((pow(seaLevelPressure/currentPressure, -5.257) - 1) * (temperature + 273.15))/(0.0065);
}


struct dps_data_struct {
  float temp = 0.0;
  float pres = 0.0;
};

/*
  ------------------------------------------------------------------
  2004A 4x16 LCD
  ------------------------------------------------------------------
*/

LCD lcd(0x27, 20, 4);


/*
  ------------------------------------------------------------------
  NEO-6M GPS
  ------------------------------------------------------------------
*/

// GPS Baud Rate
#define GPS_BAUD 9600

// Software Serial object for communciating with GPS
SoftwareSerial gps_ss(GPS_SS_RX, GPS_SS_TX);

// Tiny GPS Object
TinyGPS gps;

// variables for holding satellites and data age



struct gps_data_struct {
  uint8_t sats = TinyGPS::GPS_INVALID_SATELLITES;
  long unsigned int age = 0;
  float lat = 0;
  float lon = 0;
  float alt = 0;
};

//

/*
  ------------------------------------------------------------------
  SD CARD
  ------------------------------------------------------------------
*/

Dataset* data;

/*
  ------------------------------------------------------------------
  STATE VARIABLES
  ------------------------------------------------------------------
*/


// Enum that tracks which the device is in inside of the state machine
enum state_indicator {
  NO_GPS_LOCK,
  READY_FOR_ZERO_SET,
  CONFIRM_ZERO_SET,
  READY_FOR_LOCATION,
  CONFIRM_SAVE_LOCATION,
};


struct state {
  // Track the current and previous state
  state_indicator current = NO_GPS_LOCK;
  state_indicator previous = CONFIRM_STATE_LOCATION;

  // track how many times we have set the zero, and how many measurements have been taken
  uint8_t num_zero = 0;
  uint8_t num_measurements = 0;

  // kalman filter for the
  SimpleKalmanFilter altitude_kf = SimpleKalmanFilter(0.001, 0.01, 0.01);

  // track the most recent, last and zero gps data
  gps_data_struct gps_data_cur;
  gps_data_struct gps_data_prev;
  gps_data_struct gps_data_zero;

  // track the most recent, last and zero dps data
  dps_data_struct dps_data_cur;
  dps_data_struct dps_data_prev;
  dps_data_struct dps_data_zero;

  // track the most recent estimate of the current altitude
  float alt_esimate = 0;

  // track the current pressure at sea level
  float sea_level_pressure = 1038;

  // previous state of each button
  bool last_yes_button = true;
  bool last_no_button = true;
  bool last_card_inserted = false;
};


state device_state;


uint8_t num_loops = 0;

/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   SETUP
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/


void setup()
{

  // Setup pinmodes
  pinMode(YES_PIN, INPUT_PULLUP);
  pinMode(NO_PIN, INPUT_PULLUP);
  pinMode(SS_PIN, OUTPUT);
  pinMode(CD_PIN, INPUT);

  // Startup Serial
  Serial.begin(115200);

  // wait for serial to startup
  while (!Serial) delay(10);

  // Setup the lcd
  lcd.setup();
  lcd.startup_screen();

  // Initialize the GPS serial port
  gps_ss.begin(GPS_BAUD);

  // Initialize the dps310 sensor
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

  // done

}


/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   LOOP
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/

void loop()
{

  /*
    ------------------------------------------------------------------
    PRE-STATE ACTIONS
    ------------------------------------------------------------------
  */

  // digital inputs
  bool yes_button = !digitalRead(YES_PIN);
  bool no_button = !digitalRead(NO_PIN);
  bool card_inserted = digitalRead(CD_PIN);

  bool yes_button_changed = yes_button != device_state.last_yes_button;
  bool no_button_changed = no_button != device_state.last_no_button;
  bool card_inserted_changed = card_inserted != device_state.last_card_inserted;

  device_state.last_yes_button = yes_button;
  device_state.last_no_button = no_button;
  device_state.last_card_inserted = card_inserted;

  // is this the first time we are on this state
  bool has_state_changed = false;

  // if the state changed, update the variable and the previous
  if (device_state.current != device_state.previous) {
    device_state.previous = device_state.current;
    has_state_changed = true;
  }


  if (device_state.current > CONFIRM_ZERO_SET) {

    device_state.alt_estimate = altitude_kf.updateEstimate(getAltFromSeaLevelPressure(device_state.sea_level_pressure, device_state.dps_data_cur.pres, device_state.dps_data_cur.temp));

  }

  /*
    ------------------------------------------------------------------
    CURRENT STATE ACTIONS
    ------------------------------------------------------------------
  */


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
          lcd.print_measurement(device_state.num_zero, device_state.num_measurements, device_state.gps_data_cur.lat, device_state.gps_data_cur, recent_meas.dps_alt, recent_meas.gps_alt);
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





  /*
    ------------------------------------------------------------------
    POST STATE TRANSITIONS
    ------------------------------------------------------------------
  */

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

  delay_and_read_sensors(100);
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


static void delay_and_read_sensors(unsigned long ms) {
  unsigned long start = millis();
  do
  {

    if (current_state == READY_FOR_ZERO_SET) {

      if (dps.temperatureAvailable()) {
        dps_temperature->getEvent(&sensor_event);
        device_state.dps_data_prev.temp = device_state.dps_data_cur.temp;
        device_state.dps_data_cur.temp = sensor_event.temperature;
      }

      if (dps.pressureAvailable()) {
        dps_pressure->getEvent(&sensor_event);
        device_state.dps_data_prev.pres = device_state.dps_data_cur.pres;
        device_state.dps_data_cur.pres = sensor_event.pressure;
      }
    }


    while (gps_ss.available()) {
      gps.encode(gps_ss.read());
    }

    device_state.gps_data_prev = device_state.gps_data_cur;
    device_state.gps_data_cur.sats =  gps.satellites();
    device_state.gps_data_cur.alt = gps.f_altitude();
    gps.f_get_position(&device_state.gps_data_cur.lat, &device_state.gps_data_cur.lon, &device_state.gps_data_cur.age);

  } while (millis() - start < ms);
}
