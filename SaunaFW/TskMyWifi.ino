// ----------------------------------------------------------------------------
// My ESP WiFi module 
// functions for all basic WiFi functions e.g. connect..
// ----------------------------------------------------------------------------

#define WAIT_BETWEEN_CHECK  10000u
#define WAIT_CONNECT        1000u
#define WAIT_MIN            100u

// ----------------------------------------------------------------------------
void MyWiFi_Task(void *parameter)
{
  // task setup area
  WiFi.begin((char*)WLAN_SSID, (char*)WLAN_PWD);  
  //SP("\nConnect to WiFi ");SPLF((char*)WLAN_SSID);

  while(1)
  {
    vTaskDelay(WAIT_MIN/portTICK_PERIOD_MS);      // ensure RTOS get control

    if(WiFi.status() != WL_CONNECTED)             // not connected yet?
    {
      WLANState = WLAN_NotConnected;              // update exported state
      //SP(".");                              
      vTaskDelay(WAIT_CONNECT/portTICK_PERIOD_MS);// standard try connect symbol
    }
    if(WiFi.status() == WL_CONNECTED)             // actually connected
    {
      WLANState = WLAN_Connected;                 // update exported state
      if (LastWLANState == WLAN_NotConnected)     // change not connected => connected?
        //SPLF(" connected");                       // print message once    
      vTaskDelay(WAIT_BETWEEN_CHECK/portTICK_PERIOD_MS);  
    }
    LastWLANState = WLANState;                    // edge detection connection
  }
}