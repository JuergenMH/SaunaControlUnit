// ----------------------------------------------------------------------------
// My Sauna control IO Drive module 
// Performs these actions:
// read in and debounce all digital inputs (buttons, door contact)
// generate edge signals from input events
//
// Please consider: Evan digital input are often swichted to ground the
// module uses POSITIVE LOGIC!
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyIODrive_Init(void)                 // called once from setup routine
{
  uint8_t i;
  pinMode(INP_DOOR,  INPUT_PULLUP);       // pin is alwys input
  pinMode(INP_LIGHT, INPUT_PULLUP);       // pin is alwys input
  pinMode(INP_MODE,  INPUT_PULLUP);       // pin is alwys input
  for (i=0;i<3;bnc_dat_array[i++]=0);     // clear debounce array
}

// ----------------------------------------------------------------------------
void MyIODrive_Function(void)       // called every 5ms from task51 
{
  uint8_t inputs = 0;
  // read input signals and store them in ring buffer -------------------
  if (PIN_MODE_PRESSED)   inputs = 1;   // mode in bit 0
  if (PIN_LIGHT_PRESSED)  inputs |= 2;  // light in bit 1
  if (PIN_DOOR_CLOSED)    inputs |= 4;  // door signal in bit 2
  bnc_dat_array[bnc_cnt] = inputs ;     // store in ring buffer
  
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
  if (deb_dat_valid)
  {
    if ((0 == (deb_dat_old & 1)) && (MODE_IS_PRESSED))   flag_mode_pressed   = true;
    if ((0 != (deb_dat_old & 1)) && (!MODE_IS_PRESSED))  flag_mode_released  = true; 
    if ((0 == (deb_dat_old & 2)) && (LIGHT_IS_PRESSED))  flag_light_pressed  = true;
    if ((0 != (deb_dat_old & 2)) && (!LIGHT_IS_PRESSED)) flag_light_released = true;
    if ((0 == (deb_dat_old & 4)) && (DOOR_IS_CLOSED))    flag_door_closed    = true;
    if ((0 != (deb_dat_old & 4)) && (!DOOR_IS_CLOSED))   flag_door_opened    = true;    
    deb_dat_old = deb_dat;  // update "old" variable with current values
  }
}
// ----------------------------------------------------------------------------
void MyIODrive_ClearEdges(void)       // call at the end of one loop
{
    flag_mode_pressed   = false;            // reset all edge flags
    flag_mode_released  = false; 
    flag_light_pressed  = false;
    flag_light_released = false;
    flag_door_closed    = false;
    flag_door_opened    = false;
}

// ----------------------------------------------------------------------------
// end of the IO Drive (input debouncer) module
// ----------------------------------------------------------------------------
