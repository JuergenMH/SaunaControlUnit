// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------

const char HelloStr1[] = "Sauna Control V1.0.0";

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
