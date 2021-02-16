/*
   Metro Mini Pinout

    D0, D1 are USB Serial Debugging

    D2 through D12 are digital IO,
    D3, D5, D6, D9, D10, D11 PWM

    Our Pin Assignments

    D8 Low Battery Warning, connected to LBO pin, will be pulled to ground if battery is low (can potentionally combine this with A3 if LBO is actually connected to BAT
    D9, D10 Software Serial for GPS
    D11 MOSI, D12 MISO, D13 Clock, SPI for SD Card
    A3 Battery Voltage, connected to BAT pin on PowerBoost
    A4 Data, A5 Clock I2C for LCD, Altimeter
*/





//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <Adafruit_DPS310.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "lcd.h"

// Pressure Sensor Object
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600

// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(9, 10);

// GPS Object
TinyGPS gps;

void setup()
{

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("USB Serial Interface started");

  Serial.print("Initializing LCD Display... ");

  // Initialize the LCD, will call Wire.begin()

  lcd.setup();                      // initialize the lcd
  lcd.startup_screen();

  Serial.println("Done");

  // Initialize GPS Software Serial
  Serial.print("Initializing GPS Software Serial... ");
  gps_ss.begin(GPS_BAUD);
  Serial.println("Done");


  // Initialize Adafruit DPS310

  Serial.print("Initializing DPS310 Pressure... ");
  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    Serial.print(" ... ");
    while (1) yield();
  }
  Serial.print("Configuring... ");

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  Serial.println("Done");

  Serial.println("Printing Sensor Details");

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();





  Serial.println("Initializing Done");



}


void loop()
{

  
  unsigned long num_sats;

  num_sats = gps.satellites();

  if (num_sats == TinyGPS::GPS_INVALID_SATELLITES) {
    Serial.println("GPS has no lock");
  } else {
    Serial.print("GPS has ");
    Serial.print(num_sats);
    Serial.println(" Satellites");
  }

  sensors_event_t temp_event, pressure_event;

  if (dps.temperatureAvailable()) {
    dps_temp->getEvent(&temp_event);
    Serial.print(F("Temperature = "));
    Serial.print(temp_event.temperature);
    Serial.println(" *C");
    Serial.println();
  }

  // Reading pressure also reads temp so don't check pressure
  // before temp!
  if (dps.pressureAvailable()) {
    dps_pressure->getEvent(&pressure_event);
    Serial.print(F("Pressure = "));
    Serial.print(pressure_event.pressure);
    Serial.println(" hPa");

    Serial.println();
  }
  
  delay_and_read_gps(1000);

}


static void delay_and_read_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gps_ss.available())
      gps.encode(gps_ss.read());
  } while (millis() - start < ms);
}
