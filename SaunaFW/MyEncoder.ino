// ----------------------------------------------------------------------------
// My SaunaFW Encoder module 
// contains all encoder related functions 
// Board select: ESP32C3 Dev Module
// This module is NOT an FreeRTOS Task. It is a completely ISR driven module
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Encoder call back function for the rotary encoder turn
void MyEncoderKnobCallback( long value )
{
  TargetTemperature = (unsigned int) value;   	// set new target temperature
  flag_TargetTemperatureChanged = true;       	// set flag for main
}

// ----------------------------------------------------------------------------
// Encoder call back function for the rotary push button
void MyEncoderButtonCallback( unsigned long duration )
{
  if (duration > Temp_Store)                  	// detect long press
  {
    DEF_TEMPERATURE = TargetTemperature;   			// actual target => default
    flag_DefaultTemperatureChanged = true;    	// set flag for main
  }
}

// ----------------------------------------------------------------------------
void MyEncoderSetup() 
{
	// This tells the library that the encoder has its own pull-up resistors
	myEnc.setEncoderType( EncoderType::HAS_PULLUP );

	// Range of values to be returned by the encoder: minimum is 1, maximum is 10
	// The third argument specifies whether turning past the minimum/maximum will
	// wrap around to the other side:
	//  - true  = turn past 10, wrap to 1; turn past 1, wrap to 10
	//  - false = turn past 10, stay on 10; turn past 1, stay on 1
	myEnc.setBoundaries( Temp_Min, Temp_Max, false );

	// The function specified here will be called every time the knob is turned
	// and the current value will be passed to it
	myEnc.onTurned( &MyEncoderKnobCallback );
	
  // The function specified here will be called every time the button is pushed and
	// the duration (in milliseconds) that the button was down will be passed to it
	myEnc.onPressed( &MyEncoderButtonCallback );

	// This is where the inputs are configured and the interrupts get attached
	myEnc.begin();
}

// ----------------------------------------------------------------------------
//  end of the encoder control module
// ----------------------------------------------------------------------------
