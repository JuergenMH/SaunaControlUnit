//#include <stdio.h>
#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32RotaryEncoder.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// My SaunaFW main module 
// contains Arduino setup and loop functions
// Board select: ESP32C3 Dev Module
// ----------------------------------------------------------------------------
const char HelloStr1[] = "Sauna Control V1.0.0";
//const char HelloStr2[] = "Setup finished.";

LiquidCrystal_I2C   myLCD(LCD_ADR, LCD_WIDTH, LCD_LINES);
RotaryEncoder       myEnc(ENC_CLK, ENC_DT, ENC_SW, ENC_VCC);

// ----------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); 	
  Wire.begin(I2C_SDA, I2C_SCL); // i²C Initialisation
	myLCD.begin();                // initialize the LCD
	myLCD.backlight();            // Turn on the blacklight
	myLCD.print(HelloStr1);       //and print a message.
  EncoderSetup();               // setup rotary encoder
  delay(1000);                  // establish serial monitor
  myLCD.clear();                // prepare loop display
}

// ----------------------------------------------------------------------------
void loop() 
{
  if (TargetTemperatureChanged)
  {
    myLCD.home();
    myLCD.print(TargetTemperature);
    TargetTemperatureChanged = false;
  }
  if (DefaultTemperatureChanged)
  {
    myLCD.setCursor(0,1);
    myLCD.print(DefaultTemperature);
    DefaultTemperatureChanged = false;
  }
}
  
// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
