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
  GUI_Init();                   // init all GUI related topics
  EncoderSetup();               // setup rotary encoder  
  Relays_Init();                // switch all off
  MySystem_Init();              // init HW timer module
  MyIODrive_Init();             // init debouncer and edge handler
  MySystem_StartTimer();        // OS HW Timer start

  delay(1000);                  // establish serial monitor
}

// ----------------------------------------------------------------------------
void loop() 
{
  MySystem_Function();          // call system, scheduler and so on....
  GUI_Function();                  // all user actions in GUI function
}
  
// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
