
/*
   Metro Mini Pinout

    D0, D1 are USB Serial Debugging

    D2 through D12 are digital IO,
    D3, D5, D6, D9, D10, D11 PWM

    Our Pin Assignments

    D2 is Measure/Yes/Accept, D4 is Zero/No
    D6 Low Battery Warning, connected to LBO pin, will be pulled to ground if
   battery is low (can potentionally combine this with A3 if LBO is actually
   connected to BAT D7, D8 Software Serial for GPS D9 CD SD Card Detect D10 CS,
   D11 MOSI, D12 MISO, D13 Clock, SPI for SD Card A3 Battery Voltage, connected
   to BAT pin on PowerBoost A4 Data, A5 Clock I2C for LCD, Altimeter

    sudo chmod 777 /dev/ttyUSB0
*/

/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   INCLUDES
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/

#include "FilterButterworth.h"
#include "data.h"
#include "lcd.h"
#include "pinout.h"
#include <Adafruit_DPS310.h>
#include <SD.h>
#include <SimpleKalmanFilter.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <inttypes.h>

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

sensors_event_t sensor_event;

// https://keisan.casio.com/exec/system/1224575267
float getSeaLevelPressureFromAlt(float pressure,
                                 float height,
                                 float temperature)
{
  return (pressure)*pow(
      (1 - (0.0065 * height) / (temperature + 0.0065 * height + 273.15)),
      -5.257);
}

// https://github.com/adafruit/Adafruit_DPS310/blob/master/Adafruit_DPS310.cpp
// https://keisan.casio.com/exec/system/1224585971
float getAltFromSeaLevelPressure(float seaLevelPressure,
                                 float currentPressure,
                                 float temperature)
{
  return 44330 * (1.0 - pow(currentPressure / seaLevelPressure,
                            0.1903));
  // return ((pow(seaLevelPressure / currentPressure, -5.257) - 1) *
  //         (temperature + 273.15)) /
  //        (0.0065);
}

struct dps_data_struct
{
  float temp = 0.0;
  float pres = 0.0;
  float alt_estimate = 0.0;
};

/*
  ------------------------------------------------------------------
  2004A 4x16 LCD
  ------------------------------------------------------------------
*/

LCD lcd(0x27, 20, 4);

#define GPS_SEARCH_TEXT "GPS Search"
#define SET_ZERO_TEXT "Set Zero"
#define SET_POINT_TEXT "Set Point"

/*
  ------------------------------------------------------------------
  NEO-6M GPS
  ------------------------------------------------------------------
*/

// GPS Baud Rate
#define GPS_BAUD 9600

//
#define TIME_STEP 0.25

// Software Serial object for communciating with GPS
SoftwareSerial gps_ss(GPS_SS_RX, GPS_SS_TX);

// Tiny GPS Object
TinyGPS gps;

// variables for holding satellites and data age

struct gps_data_struct
{
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

Dataset *data;

/*
  ------------------------------------------------------------------
  STATE VARIABLES
  ------------------------------------------------------------------
*/

// Enum that tracks which the device is in inside of the state machine
enum state_indicator
{
  NO_SD,
  READY_FOR_START,
  DEVICE_RUNNING,
  SAVE_POINT,
};

struct state
{
  // Track the current and previous state
  state_indicator current = NO_SD;
  state_indicator previous = READY_FOR_START;

  // track how many times we have set the zero, and how many measurements have
  // been taken
  uint8_t num_zero = 0;
  uint8_t num_measurements = 0;

  // kalman filter for the
  SimpleKalmanFilter altitude_kf = SimpleKalmanFilter(0.001, 0.01, 0.01);

  // previous state of each button
  bool last_yes_button = true;
  bool last_no_button = true;
  bool last_card_inserted = false;

  float last_pressure = 0.0;

  bool data_writing = false;

  bool request_data = false;

  float time = 0.0;
};

state device_state;

uint8_t num_loops;

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
  while (!Serial)
    delay(10);

  // Setup the lcd
  lcd.setup();
  lcd.startup_screen();

  // Initialize the dps310 sensor
  if (!dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire))
  {
    ;
  }

  // Setup highest precision
  dps.setMode(DPS310_CONT_PRESSURE);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);

  // SD Card
  if (SD.begin(SS_PIN))
  {
  }

  data = new Dataset;


  cli(); 
  TCCR1A = 0; 
  TCCR1B = 0; 
  OCR1A = 62499; // = 16000000 / (64 * 4) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10); // Prescalar = 64
  TIMSK1 |= (1 << OCIE1A); // enable interrupt
    
  TCNT1  = 0; // counter value initialized to 0
  sei(); // allow interrupts

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
  if (device_state.current != device_state.previous)
  {
    device_state.previous = device_state.current;
    has_state_changed = true;
    lcd.clear();
  }

  bool do_screen_update = (num_loops % 40) == 0;


  /*
    ------------------------------------------------------------------
    CURRENT STATE ACTIONS
    ------------------------------------------------------------------
  */

  // do current state actions
  switch (device_state.current)
  {
  case NO_SD:
  {
    // update lcd
    if (has_state_changed)
    {
      lcd.no_sd_screen();

    }

    break;
  }
  case READY_FOR_START:
  {

    // update the screen with the current data coming from the sensors

    if (has_state_changed)
    {
      lcd.ready_to_start_screen();
    }


    break;
  }
  case DEVICE_RUNNING:
  {

    if (do_screen_update) {
      lcd.clear();
    }

    

    break;
  }
  case SAVE_POINT:
  {
    if (has_state_changed) {
      device_state.request_data = true;
      lcd.writing_screen();
    }
  }
  }

  /*
    ------------------------------------------------------------------
    POST STATE TRANSITIONS
    ------------------------------------------------------------------
  */

  // do current state actions
  bool was_yes_pressed = yes_button && yes_button_changed;
  bool was_no_pressed = no_button && no_button_changed;

  switch (device_state.current)
  {
  case NO_SD:
  {

    if (card_inserted)
      device_state.current = READY_FOR_START;

    break;
  }
  case READY_FOR_START:
  {

    if (was_yes_pressed)
    {
      device_state.current = DEVICE_RUNNING;
    }

    break;
  }
  case DEVICE_RUNNING:
  {

    // set initial text
    if (yes_button_changed && yes_button) {
      device_state.current = SAVE_POINT;
    } 

    break;
  }
  case SAVE_POINT:
  {
    if (!device_state.request_data) {
      device_state.current = DEVICE_RUNNING;
    }
  }
  }

  // wait

  if (has_state_changed || (num_loops % 40 == 0))
  {
    lcd.setTopStatusText(F("ISWSD"));
    lcd.setTopStatusIndiciators(data->filename, 0);
  }

  if (num_loops % 20 == 0)
    lcd.progress_loop(6, 0, 1);

  delay_and_read_sensors(50);
  num_loops++;
}

// void record_measurement(float val1, float val2, float val3)
// {
//   Serial.print("\t");

//   Serial.print(val1);

//   Serial.print(" ");
//   Serial.print(val2);

//   Serial.print(" ");
//   Serial.println(val3);
// }

static void delay_and_read_sensors(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    
    if (!device_state.data_writing && dps.pressureAvailable()) {
      dps_pressure->getEvent(&sensor_event);
      device_state.last_pressure = sensor_event.pressure;
    }


  } while (millis() - start < ms);
}


ISR(TIMER1_COMPA_vect){ 
  Serial.println("saving data");
  device_state.data_writing = true;
  device_state.time += TIME_STEP;
  data->write_data(device_state.time,device_state.request_data,device_state.last_pressure);

  if (device_state.request_data)
    device_state.request_data = false;

  device_state.data_writing = false;

  //Serial.println(TCNT1*.000004,3);

}