//#include <stdio.h>
#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// My SaunaFW main module 
// contains Arduino setup and loop functions
// Board select: ESP32C3 Dev Module
// ----------------------------------------------------------------------------
const char HelloStr1[] = "Sauna Control V1.0.0";
const char HelloStr2[] = "Setup finished.";

LiquidCrystal_I2C   myLCD(LCD_ADR, LCD_WIDTH, LCD_LINES);
Encoder             myEnc(ENC_DT, ENC_CLK);

// ----------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); 	
  Wire.begin(I2C_SDA, I2C_SCL); // i²C Initialisation
	myLCD.begin();                // initialize the LCD
	myLCD.backlight();            // Turn on the blacklight
	myLCD.print(HelloStr1); //and print a message.
  delay(1000);                  // establish serial monitor

}

// ----------------------------------------------------------------------------
unsigned int Temperature = 50;

void loop() 
{
  static long newPos; 
  static long EncPos = -999;
  static float TempF = 50.0;
  
  newPos = myEnc.read();
  if (newPos > EncPos)
  {
    TempF = TempF +0.2;
    if (TempF > 100.0) TempF = 100.0;
  }
  if (newPos < EncPos)
  {
    TempF = TempF -0.2;
    if (TempF < 40.0) TempF = 40.0;
  } 
  if ((unsigned int)TempF != Temperature) 
  {
    Temperature = (unsigned int) TempF;
    Serial.println(Temperature);   
  }
    EncPos = newPos;
}
  
// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
