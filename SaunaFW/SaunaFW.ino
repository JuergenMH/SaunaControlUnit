#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32RotaryEncoder.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// My SaunaFW main module(s)
// contains Arduino setup and loop functions
// Board select: ESP32C3 Dev Module
// ----------------------------------------------------------------------------

//#define CHK_WATERMARK_ON          // if defined watermark (stack) check enabled

// ----------------------------------------------------------------------------
// String area
// ----------------------------------------------------------------------------
const char HelloStr[]     = "Sauna Control V1.0.0 (FreeRTOS)";
const char SetupFinish[]  = "Setup finished.";

// ----------------------------------------------------------------------------
// define task handles, used for communication with the tasks
// ----------------------------------------------------------------------------
TaskHandle_t KTY_TskHandle;       // KTY measurment task 
TaskHandle_t IODrvive_TskHandle;  // IO Drive task
TaskHandle_t Relay1_TskHandle;    // Task for relay 1
TaskHandle_t Relay2_TskHandle;    // ditto relay 2
TaskHandle_t Relay3_TskHandle;    // ditto relay 3
TaskHandle_t Wifi_TskHandle;      // Wifi task
TaskHandle_t WM_TskHandle;        // Stack measurement task

// ----------------------------------------------------------------------------
// Sub functions of setup and loop
// ----------------------------------------------------------------------------
void HandleConfiguration(void)
{
  MyConfig_ReadConfig();                    // initial eprom read
  while (EE_Valid != ConfigState)           // if not OK...
  {
    MyConfig_PerformConfigDialog();         // enter config dialog
    MyConfig_ReadConfig();                  // read eeprom againg
  }                                         // until content is valid
  //MyConfig_PerformConfigDialog();         // for testing
  TargetTemperature = DEF_TEMPERATURE;      // set actual = default value
  myEnc.setEncoderValue(TargetTemperature); // also in encoder module
}

// ----------------------------------------------------------------------------
// Check task to control stack requirement for ever task
// only temporarely used to determine the required stack size for ervery task
// control by #define at the beginning of this module
// ----------------------------------------------------------------------------
void printWatermark(void *pvParameters)
{
    while(1)
    {
        delay(2000);
        SP(pcTaskGetName(KTY_TskHandle));      SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(KTY_TskHandle));
        SP(pcTaskGetName(IODrvive_TskHandle)); SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(IODrvive_TskHandle));
        SP(pcTaskGetName(Relay1_TskHandle));   SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(Relay1_TskHandle));
        SP(pcTaskGetName(Relay2_TskHandle));   SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(Relay2_TskHandle));
        SP(pcTaskGetName(Relay3_TskHandle));   SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(Relay3_TskHandle));
      //SP(pcTaskGetName(Wifi_TskHandle));     SP(", STACK: "); SPLF(uxTaskGetStackHighWaterMark(Wifi_TskHandle));
        SPLF("");
    }
}

// ----------------------------------------------------------------------------
// Start all necessary tasks and assign task handler
// ----------------------------------------------------------------------------
void StartAllTasks(void)
{
            //fct name        descr. name,  Stack parameter priority handle
  xTaskCreate(MyKTY_Task,     "KTY calc",   1500, NULL,     1,       &KTY_TskHandle);
  xTaskCreate(MyIODrive_Task, "IODrive",    1000, NULL,     1,       &IODrvive_TskHandle);
  xTaskCreate(MyRelay_1_Task, "Relay 1",    1200, NULL,     1,       &Relay1_TskHandle);
  xTaskCreate(MyRelay_2_Task, "Relay 2",    1200, NULL,     1,       &Relay2_TskHandle);
  xTaskCreate(MyRelay_3_Task, "Relay 3",    1200, NULL,     1,       &Relay3_TskHandle);
//  xTaskCreate(MyWiFi_Task,    "WiFi tsk",   8196, NULL,     1,       &Wifi_TskHandle);
  
  #ifdef CHK_WATERMARK_ON
    xTaskCreate(printWatermark, "Watermark",  2048, NULL,     1,       &WM_TskHandle);
  #endif  
}

// ----------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); 	
  MyEncoderSetup();             // setup rotary encoder  
  delay(1000);                  // establish serial monitor
  Serial.println(HelloStr);     // first hello on serial console
  HandleConfiguration();        // read configuration, start configurator if necessary
  StartAllTasks();              // start all tasks
  Serial.println(SetupFinish);  // end of arduino setup function; enter main loop



  //MyGUI_Init();                 // init all GUI related topics
  //MyRelays_Init();              // switch all off
  //MySystem_Init();              // init HW timer module
  //MyIODrive_Init();             // init debouncer and edge handler
  //MyKTY_Init();                 // prepare KTY analogue measurment
 
  //MyWifiHandler_Init();         // Connect WiFi 
  //MyNTPHandler_Init();          // Establish NTP serive WLAN connected and NTP enabled
  //MySystem_StartTimer();        // OS HW Timer start
  //MyGUI_Prepare_LCD();          // write constant text to LCD
  
}

// ----------------------------------------------------------------------------
void loop() 
{
    // At this moment test code only

  /*
  if(flag_TargetTemperatureChanged)
  {
    Serial.println(TargetTemperature);
    flag_TargetTemperatureChanged = false;
  }
  // WERTE STIMMEN NOCH NICHT

  if(flag_NewTemperatureAvailable)
  {
    Serial.print("Room:  "); Serial.println(RoomTemperature);
    Serial.print("Sauna: "); Serial.println(SaunaTemperature);
    flag_NewTemperatureAvailable = false;
  }
      delay(500);
      if (flag_mode_pressed)    Serial.println ("Mode pressed");
      if (flag_mode_released)   Serial.println ("Mode released"); 
      if (flag_light_pressed)   Serial.println ("Light pressed");
      if (flag_light_released)  Serial.println ("Light released");
      if (flag_door_closed)     Serial.println ("Door closed");
      if (flag_door_opened)     Serial.println ("Door opened");    

  */
  
    delay(500);

    if (flag_mode_pressed && REL_1_IS_OFF)  {MyRelays_Function(Cmd_Relay1_On); Serial.println("Switch relay 1 on");}
    if (flag_mode_pressed && REL_1_IS_ON)   {MyRelays_Function(Cmd_Relay1_Off);Serial.println("Switch relay 1 off");}

    if (flag_light_pressed && REL_3_IS_OFF)  {MyRelays_Function(Cmd_Relay3_On); Serial.println("Switch relay 3 on");}
    if (flag_light_pressed && REL_3_IS_ON)   {MyRelays_Function(Cmd_Relay3_Off);Serial.println("Switch relay 3 off");}
  

      MyIODrive_ClearEdges();
}
  
// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
