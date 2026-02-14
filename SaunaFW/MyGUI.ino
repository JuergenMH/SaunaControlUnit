// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------
const char LCD_1[] = "dd.mm.yyyy hh:mm    ";
const char LCD_2[] = "Room :xxX|Light:xxx ";
const char LCD_3[] = "Sauna:xxX|Door :OK  ";
const char LCD_4[] = "Targ.:xxX|State:Wait";

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
void MyGUI_Prepare_LCD(void)
{
  myLCD.home();
  myLCD.print(LCD_1);
  myLCD.print(LCD_2);
  myLCD.print(LCD_3);
  myLCD.print(LCD_4);

}

// ----------------------------------------------------------------------------
void MyGUI_Init(void)
{
  Wire.begin(I2C_SDA, I2C_SCL); // i²C Initialisation
	myLCD.begin();                // initialize the LCD
	myLCD.backlight();            // Turn on the blacklight
	myLCD.print(HelloStr1);       //and print a message.
  
}

// ----------------------------------------------------------------------------
void MyGUI_Function(void)
{
  Handle_Light_Button();        // switch light on/off


}

// ----------------------------------------------------------------------------
// end of the GUI module
// ----------------------------------------------------------------------------
