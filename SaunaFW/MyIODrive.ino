// ----------------------------------------------------------------------------
// My Sauna control IO Drive module 
// Performs these actions:
// read in and debounce all digital inputs (buttons, door contact)
// generate edge signals from input events
//
// Please consider: Evan digital input are often swichted to ground the
// module uses POSITIVE LOGIC!
// ----------------------------------------------------------------------------

#define MODE_PRESSED    (false == digitalRead(INP_MODE))
#define LIGHT_PRESSED   (false == digitalRead(INP_LIGHT))
#define DOOR_CLOSED     (false == digitalRead(INP_DOOR))

// ----------------------------------------------------------------------------
void MyIODrive_Init(void)           // called once from setup routine
{
  uint8_t i;
  pinMode(INP_DOOR, INPUT);         // pin is alwys input
  digitalWrite(INP_DOOR, HIGH);     // enable pull up resistor
  pinMode(INP_LIGHT, INPUT);        // pin is alwys input
  digitalWrite(INP_LIGHT, HIGH);    // enable pull up resistor
  pinMode(INP_MODE, INPUT);         // pin is alwys input
  digitalWrite(INP_MODE, HIGH);     // enable pull up resistor
  for (i=0;i<3;bnc_dat_array[i++]=0);
}

// ----------------------------------------------------------------------------
void MyIODrive_Function(void)       // called every 5ms from task51 
{
  uint8_t inputs = 0;
  // read input signals an store in ring buffer ------------------------
  if (MODE_PRESSED)   inputs = 1;   // mode in bit 0
  if (LIGHT_PRESSED)  inputs |= 2;  // light in bit 1
  if (DOOR_CLOSED)    inputs |= 4;  // door signal in bit 2
  bnc_dat_array[bnc_cnt] = inputs ; // store in ring buffer
  
  // debounce step 1, 2, 3 ---------------------------------------------
  // set bit to "1" if not 3 times equal
  bnc_dat  = bnc_dat_array[0] ^ bnc_dat_array[1] ; // step 1
  bnc_dat |= bnc_dat_array[1] ^ bnc_dat_array[2] ; // step 2
  bnc_dat |= bnc_dat_array[2] ^ bnc_dat_array[0] ; // step 3

  // set bit to "1" if not 3 times equal --------------------------------
  deb_dat &= bnc_dat ;        				    // reset constant bits
	deb_dat |= bnc_dat_array[0] & ~bnc_dat; // set constant bits

  // handle cyclic counter ---------------------------------------------
  bnc_cnt++;
  if (3 == bnc_cnt)       // 3 = overflow
  {
    bnc_cnt = 0;          // reset cycle counter to zero
    deb_dat_valid = true; // at least one cycle is finished
  }
  
  // edge generation module --------------------------------------------
  if ((0 == (deb_dat_old && 1)) && (MODE_IS_PRESSED))   mode_pressed = true;
  if ((0 != (deb_dat_old && 1)) && (~MODE_IS_PRESSED))  mode_released = true; 
  if ((0 == (deb_dat_old && 2)) && (LIGHT_IS_PRESSED))  light_pressed = true;
  if ((0 != (deb_dat_old && 2)) && (~LIGHT_IS_PRESSED)) light_released = true;
  deb_dat_old = deb_dat;  // update "old" variable with current values
}

// ----------------------------------------------------------------------------
