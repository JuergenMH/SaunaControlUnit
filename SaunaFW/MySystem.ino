// ----------------------------------------------------------------------------
// My Sauna control System module 
// Hardware- and software timer, scheduler
// for timer refer: https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
// ----------------------------------------------------------------------------

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

// ----------------------------------------------------------------------------
// Low level tasks, called directly by the scheduler at the end
// ----------------------------------------------------------------------------
void MyApp_1msTask(void)
{
  #ifdef MEASURE_TASK_1
    SET_DEBUG_1(true);
  #endif	
  // handle the software timer
  if (SW_Timer_1) SW_Timer_1--;
  if (SW_Timer_2) SW_Timer_2--;
  if (SW_Timer_3) SW_Timer_3--;

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
    MyIODrive_Function();   // input debouncer
  #ifdef MEASURE_TASK_51
    SET_DEBUG_1(false);
  #endif	
}

// ----------------------------------------------------------------------------
void MyApp_5msTask_2(void)
{
  #ifdef MEASURE_TASK_52    // last measurement 14.02.2026 100µs
    SET_DEBUG_1(true);
  #endif	
    MyKTY_Function();       // temperature measurement of both channels
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
    GUI_Function();
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
void MySystem_Scheduler(void)
{
  static volatile unsigned int TskSelect;
  MyApp_1msTask();
  switch (TskSelect)
  {
    case 0: MyApp_5msTask_1(); break;
    case 1: MyApp_5msTask_2(); break;
    case 2: MyApp_5msTask_3(); break;
    case 3: MyApp_5msTask_4(); break;
    case 4: MyApp_5msTask_5(); break;
  }
  if (++TskSelect == 5) 
  {
    TskSelect = 0;
    MyIODrive_ClearEdges();
  }
}

// ----------------------------------------------------------------------------
void ARDUINO_ISR_ATTR onTimer(void) 
{
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

// ----------------------------------------------------------------------------
// Interface to the software
// ----------------------------------------------------------------------------
void MySystem_Init(void)
{
  // init the two debug pins
  pinMode(DEBUG_1, OUTPUT); digitalWrite(DEBUG_1, 0);
  pinMode(DEBUG_2, OUTPUT); digitalWrite(DEBUG_2, 0);
  // init the hardware timer for 1ms  
  timerSemaphore = xSemaphoreCreateBinary();  // create semaphore
  timer = timerBegin(1000000);                // timmer clock 1MHz
  timerAttachInterrupt(timer, &onTimer);      // connect ISR to timer  
}

// ----------------------------------------------------------------------------
void MySystem_StartTimer(void)
{
  // start timer with 1KHz, autoreload, endless
  timerAlarm(timer, 1000, true, 0);           
}

// ----------------------------------------------------------------------------
void MySystem_StopTimer(void)
{
  // start timer with 1KHz, autoreload, endless
  timerAlarm(timer, 0, false, 0);           
}

// ----------------------------------------------------------------------------
void MySystem_Function(void)                        // system background functions
{
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)  // timer isr occured?
  {  
    MySystem_Scheduler();                           // Call scheduler
  }
  else                                              // no timer occured
  {
    MyApp_BkgTask();                                // call the background task
  }
}

// ----------------------------------------------------------------------------
// end of the system control module
// ----------------------------------------------------------------------------
