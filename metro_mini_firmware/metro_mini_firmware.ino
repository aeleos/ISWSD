/*
   Metro Mini Pinout

    D0, D1 are USB Serial Debugging

    D2 through D12 are digital IO,
    D3, D5, D6, D9, D10, D11 PWM

    Our Pin Assignments

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

// Pressure Sensor Object
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

// Liquid Crystal Display Object
LCD lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define GPS_BAUD 9600
#define BAT_PIN 3
// Software Serial Object for GPS, (rx, tx)
SoftwareSerial gps_ss(7, 8);

// GPS Object
TinyGPS gps;

int voltage_to_percent(float volt){
  int percent = int((4.2-2.8)*72.4286);
  if (percent > 100)
    percent = 100;
    if (percent < 00)
    percent = 00;
  return percent;
}

void setup()
{

  // Startup Serial
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("USB SI started..."));

  Serial.print(F("LCD init... "));

  // Initialize the LCD, will call Wire.begin()

  lcd.setup();                      // initialize the lcd
  //lcd.startup_screen();

  //  lcd.standard_screen(0,2);
    //lcd.print_measurement(0, 2, -42.567, -10.532, 35.62);
    //lcd.print_battery(10,1);

  Serial.println(F("Done"));

  // Initialize GPS Software Serial
  Serial.print(F("GPS init... "));
  gps_ss.begin(GPS_BAUD);
  Serial.println(F("Done"));


  // Initialize Adafruit DPS310

  Serial.print(F("DPS310 init... "));
  if (! dps.begin_I2C(DPS310_I2CADDR_DEFAULT, &Wire)) {
    Serial.print(" ... ");
//    while (1) y/ield();
  }
  Serial.print(F("Config... "));

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  Serial.println(("Done"));

  Serial.println(("Sensor Details"));

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();





  Serial.println(("Init. Done"));

}


void loop()
{

 float voltage = (float)analogRead(BAT_PIN) * 5.0/1024.0;
 bool card = 0; // is the sd card present
  Serial.println(voltage);


  unsigned long num_sats;

  num_sats = gps.satellites();
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;

  gps.stats(&chars, &sentences, &failed);



  Serial.println(chars);
  Serial.println(sentences);
  Serial.println(failed);

  lcd.gpslock_screen(num_sats, TinyGPS::GPS_INVALID_SATELLITES);
  lcd.top_bar(voltage_to_percent(voltage),card);
  
  if (num_sats == TinyGPS::GPS_INVALID_SATELLITES) {
    Serial.println(F("GPS has no lock"));
  } else {
    Serial.print(("GPS: "));
    Serial.print(num_sats);
    Serial.println((" Sats"));
  }

  sensors_event_t temp_event, pressure_event;

  if (dps.temperatureAvailable()) {
    dps_temp->getEvent(&temp_event);
    Serial.print(F("Temp = "));
    Serial.print(temp_event.temperature);
    Serial.println(F(" *C"));
    Serial.println();
  }

  // Reading pressure also reads temp so don't check pressure
  // before temp!
  if (dps.pressureAvailable()) {
    dps_pressure->getEvent(&pressure_event);
    Serial.print(F("Pres. = "));
    Serial.print(pressure_event.pressure);
    Serial.println(F(" hPa"));

    Serial.println();
  }
  lcd.progress_loop(13,0,1);
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
