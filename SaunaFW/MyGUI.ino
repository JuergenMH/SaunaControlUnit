// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------
// from whatever reason; middle lines must be swapped...
const char LCD_1[] = "  .  .       :      ";
const char LCD_3[] = "Room :   |Light:    ";
const char LCD_2[] = "Sauna:   |Door :    ";
const char LCD_4[] = "Targ.:   |State:    ";

byte GradCelcius[8] = 
{
  B11000, B11000, B00000, B01111, B01000, B01000, B01000, B01111
};

byte Heater[8] =
{
  B10010,
  B01101,
  B10010,
  B01101,
  B10010,
  B11111,
  B11111,
  B01110
};

// ----------------------------------------------------------------------------
// Sub functions of GUI module

void Handle_Light_Button(void)
{
  if (deb_dat_valid && flag_light_pressed)
  {
    myLCD.setCursor(16, 1);
    if (FSM_Relay_Off == Relay3_FSM)
    {
      MyRelays_Function(Cmd_Relay3_On);
      myLCD.print("On ");
    }
    if (FSM_Relay_Off < Relay3_FSM)
    {
      MyRelays_Function(Cmd_Relay3_Off);
      myLCD.print("Off");
    }
  }  
}

// ----------------------------------------------------------------------------
void MyGUI_Update_LCDState(void)
{
// erst mal zum Testen; muss später anders gelöst werden; dauert viel zu lange.

  myLCD.setCursor(6,1); myLCD.print(RoomTemperature);
  myLCD.setCursor(6,2); myLCD.print(SaunaTemperature);
  myLCD.setCursor(6,3); myLCD.print(TargetTemperature);

  myLCD.setCursor(16, 1);
  if (FSM_Relay_Off < Relay3_FSM) myLCD.print("On");
  else                            myLCD.print("Off");  

  myLCD.setCursor(16, 2);
  if (DOOR_IS_CLOSED) myLCD.print("OK  ");
  else                myLCD.print("Open");  

}
// ----------------------------------------------------------------------------

void MyGUI_Prepare_LCD(void)  // time 140ms!
{
  //SET_DEBUG_1(true);
  myLCD.home();
  myLCD.print(LCD_1);
  myLCD.print(LCD_2);
  myLCD.print(LCD_3);
  myLCD.print(LCD_4);

  myLCD.setCursor(8,1);  myLCD.write(2);  // °C
  myLCD.setCursor(8,2);  myLCD.write(2);
  myLCD.setCursor(8,3);  myLCD.write(2);
  myLCD.setCursor(19,0); myLCD.write(3);  // heater

  //SET_DEBUG_1(false);
}

// ----------------------------------------------------------------------------

void MyGUI_Init(void)
{
  Wire.begin(I2C_SDA, I2C_SCL); // i²C Initialisation
  Wire.setClock(I2C_CLOCK);     // 400KHz
	myLCD.begin();                // initialize the LCD
	myLCD.backlight();            // Turn on the blacklight
	myLCD.print(HelloStr1);       //and print a message.

  myLCD.createChar(2,GradCelcius);
  myLCD.createChar(3,Heater);

}

// ----------------------------------------------------------------------------
void MyGUI_Function(void)
{
  Handle_Light_Button();        // switch light on/off
  MyGUI_Update_LCDState();
}

// ----------------------------------------------------------------------------
// end of the GUI module
// ----------------------------------------------------------------------------
