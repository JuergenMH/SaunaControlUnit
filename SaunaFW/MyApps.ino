// ----------------------------------------------------------------------------
// My Sauna control application tasks module 
// Tasks called from scheduler in system module
// - one 1ms Task
// - five 5ms Tasks
// - one background task
// => all tasks must be cooperative!
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Sub function call from the tasks below
// ----------------------------------------------------------------------------
void HandleLightButton(void)
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
// Low level tasks, called directly by the scheduler
// ----------------------------------------------------------------------------
void MyApp_1msTask(void)
{
  #ifdef MEASURE_TASK_1
    SET_DEBUG_1(true);
  #endif	
  #ifdef MEASURE_TASK_1
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_1(void)
{
  #ifdef MEASURE_TASK_51    // last measurement 13.02.2026 2,8µs
    SET_DEBUG_1(true);
  #endif	
  MyIODrive_Function();     // input debouncer
  HandleLightButton();      // perform light on/off
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
  MyRelay_FSM();            // operate all relays
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
  #ifdef MEASURE_TASK_BKG
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
// end of the Apps module; apps are called from main scheduler in MySystem.ini
// ----------------------------------------------------------------------------
