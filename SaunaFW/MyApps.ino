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
  #ifdef MEASURE_TASK_1
    SET_DEBUG_1(true);
  #endif	
  MyIODrive_ClearEdges();
  #ifdef MEASURE_TASK_1
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_1(void)
{
  #ifdef MEASURE_TASK_51
    SET_DEBUG_1(true);
  #endif	
  MyIODrive_Function();     // input debouncer
  #ifdef MEASURE_TASK_51
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_2(void)
{
  #ifdef MEASURE_TASK_52
    SET_DEBUG_1(true);
  #endif	
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
  #ifdef MEASURE_TASK_52
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_3(void)
{
  #ifdef MEASURE_TASK_53
    SET_DEBUG_1(true);
  #endif	
  #ifdef MEASURE_TASK_53
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_4(void)
{
  #ifdef MEASURE_TASK_54
    SET_DEBUG_1(true);
  #endif	
  #ifdef MEASURE_TASK_54
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_5(void)
{
  #ifdef MEASURE_TASK_55
    SET_DEBUG_1(true);
  #endif	
  MyIODrive_ClearEdges();
  #ifdef MEASURE_TASK_55
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_BkgTask(void)
{
  #ifdef MEASURE_TASK_BKG
    SET_DEBUG_1(true);
  #endif	
  MyIODrive_ClearEdges();
  #ifdef MEASURE_TASK_BKG
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
