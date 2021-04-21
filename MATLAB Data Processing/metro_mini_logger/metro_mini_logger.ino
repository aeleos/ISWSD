
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

#include "lcd.h"
#include <Adafruit_DPS310.h>
#include <SPI.h>
#include <SD.h>
//#include <SimpleKalmanFilter.h>

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

sensors_event_t temp_event,press_event;

//SimpleKalmanFilter altitude_kf = SimpleKalmanFilter(.1, .1, 0.01);


char filename[8] = "XXX.csv";
float value;

/*
  ------------------------------------------------------------------
  2004A 4x16 LCD
  ------------------------------------------------------------------
*/

LCD lcd(0x27, 20, 4);

/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   SETUP
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/

void setup()
{   
   randomSeed(analogRead(2));
   pinMode(2, INPUT_PULLUP);

  

  // Startup Serial
  Serial.begin(115200);

  // wait for serial to startup
  while (!Serial)
    delay(10);

  // Setup the lcd
  lcd.setup();

  // Initialize the dps310 sensor
  if (!dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire))
  {
    ;
  }

  // Setup highest precision
  dps.setMode(DPS310_CONT_PRESSURE);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);

  //Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  //Serial.println("card initialized.");


  unsigned int file_number = random(0,999);
  int i = file_number % 100;
  int j = i % 10;
  filename[2] = j + 0x30;
  filename[1] = int((i - j) / 10) + 0x30;
  filename[0] = int((file_number - i - j) / 100) + 0x30;
  File myFile = SD.open(filename, FILE_WRITE);
  //Serial.println(filename);
  myFile.println("hPa");
  myFile.close();
  

  // TIMER 1 for interrupt frequency 4 Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    // set compare match register for 4 Hz increments
    OCR1A = 62499; // = 16000000 / (64 * 4) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 64 prescaler
    TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    while (!digitalRead(2));
    //Serial.println("Beginning logging");
    TCNT1  = 0; // initialize counter value to 0
    sei(); // allow interrupts

  // done
}

ISR(TIMER1_COMPA_vect){ 
          File myFile = SD.open(filename, FILE_WRITE);
          myFile.println(value,6);
          myFile.close();
          //Serial.println(TCNT1*.000004,3);

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
  if (dps.temperatureAvailable())
  {
    dps_temperature->getEvent(&temp_event);
    dps_pressure->getEvent(&press_event);
  }
  value = press_event.pressure;
  delay(.2);
}
