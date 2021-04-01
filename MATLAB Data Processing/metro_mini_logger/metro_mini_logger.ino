
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



/*
  ------------------------------------------------------------------
  2004A 4x16 LCD
  ------------------------------------------------------------------
*/

LCD lcd(0x27, 20, 4);



bool logging = 0;
unsigned int loop_count = 0;
unsigned int log_on_count;

/*
   ------------------------------------------------------------------
   ------------------------------------------------------------------
   SETUP
   ------------------------------------------------------------------
   ------------------------------------------------------------------
*/

void setup()
{

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
  lcd.progress_loop(0,0,1);

  if (logging){

    if (loop_count == log_on_count){
    
      if (dps.temperatureAvailable())
        {
          dps_temperature->getEvent(&temp_event);
          Serial.println(temp_event.temperature);
        }   
      else
        {
          Serial.println("NaN");
        }

      if (dps.pressureAvailable())
        {
          dps_pressure->getEvent(&press_event);
          Serial.println(press_event.pressure);
        }
      else
        {
          dps_pressure->getEvent(&press_event);
          Serial.println(press_event.pressure);
        }
    
      loop_count=0;
    
    }
    
    loop_count++;
      
  }


  if (Serial.available()){
    
    int serial_read = Serial.parseInt();
    
    if (!serial_read){
      logging = 0;
      }
    else{
      logging = 1;
      log_on_count = serial_read;
      loop_count = log_on_count;
      }
    lcd.print(serial_read);
    while (Serial.available())
      serial_read = Serial.read();
  }

  delay(1000);
}
