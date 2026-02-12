// ----------------------------------------------------------------------------
// My Sauna control application tasks module 
// Tasks called from scheduler in system module
// - one 1ms Task
// - five 5ms Tasks
// - one background task
// => all tasks must be cooperative!
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyApp_1msTask(void)
{

}

// ----------------------------------------------------------------------------
void MyApp_5msTask_1(void)
{
  MyIODrive_Function();     // input debouncer
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_2(void)
{
   // Testcode für debouncer and edge detction
  if (deb_dat_valid)
  {
    if (flag_mode_pressed)   Serial.println("Mode pressed.");
    if (flag_mode_released)  Serial.println("Mode released.");
    if (flag_light_pressed)  Serial.println("Light pressed.");
    if (flag_light_released) Serial.println("Light released.");
    if (flag_door_opened)    Serial.println("Door opened");
    if (flag_door_closed)    Serial.println("Door closed");
  }
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_3(void)
{

}

// ----------------------------------------------------------------------------
void MyApp_5msTask_4(void)
{

}

// ----------------------------------------------------------------------------
void MyApp_5msTask_5(void)
{
  MyIODrive_ClearEdges();
}

// ----------------------------------------------------------------------------
void MyApp_BkgTask(void)
{

}
// ----------------------------------------------------------------------------
