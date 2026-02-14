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

// ----------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); 	
  EncoderSetup();               // setup rotary encoder  
  MyGUI_Init();                 // init all GUI related topics
  Relays_Init();                // switch all off
  MySystem_Init();              // init HW timer module
  MyIODrive_Init();             // init debouncer and edge handler
  MyKTY_Init();                 // prepare KTY analogue measurment
  MySystem_StartTimer();        // OS HW Timer start
  delay(1000);                  // establish serial monitor
  MyGUI_Prepare_LCD();          // write constant text to LCD
  Serial.println(HelloStr1);
  Serial.println("Setup finished...");
}

// ----------------------------------------------------------------------------
void loop() 
{
  MySystem_Function();          // call system, scheduler and so on....
}
  
// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
