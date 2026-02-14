// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------
const char HelloStr1[] = "Sauna Control V1.0.0";

// ----------------------------------------------------------------------------
// Sub functions of GUI module

void Handle_Light_Button(void)
{
  if (deb_dat_valid && flag_light_pressed)
  {
    if (FSM_Relay_Off == Relay3_FSM)
    {
      Relays_Function(Cmd_Relay3_On);
    }
    if (FSM_Relay_Off < Relay3_FSM)
    {
      Relays_Function(Cmd_Relay3_Off);
    }
  }  

}
// ----------------------------------------------------------------------------
void GUI_Init(void)
{
  Wire.begin(I2C_SDA, I2C_SCL); // i²C Initialisation
	myLCD.begin();                // initialize the LCD
	myLCD.backlight();            // Turn on the blacklight
	myLCD.print(HelloStr1);       //and print a message.
  
}

// ----------------------------------------------------------------------------
void GUI_Function(void)
{
  Handle_Light_Button();        // switch light on/off

}

// ----------------------------------------------------------------------------
// end of the GUI module
// ----------------------------------------------------------------------------
