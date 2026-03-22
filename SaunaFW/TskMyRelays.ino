// ----------------------------------------------------------------------------
// My Sauna control Relay handler module 
// Performs all relay functions (two relay FSMs)
// ----------------------------------------------------------------------------

#define REL_PWM_HIGH_VALUE  205     // 90% * 2,55 higher PWM value digits
#define REL_PWM_LOW_VALUE   127     // 50% * 2,55 final PWM value in digits
#define REL_PWM_HIGH_TIME   50      // time for high PWM in ms
#define REL_MIN_OFF_TIME    50      // ensure relay is switched off

// interface between command interface and tasks
bool Req_Rel1_on  = false;
bool Req_Rel1_off = false;
bool Req_Rel2_on  = false;
bool Req_Rel2_off = false;
bool Req_Rel3_on  = false;
bool Req_Rel3_off = false;

// ----------------------------------------------------------------------------
// Relay control task (based on simple FSMs
// ----------------------------------------------------------------------------
void MyRelay_1_Task(void *parameter)
{
  // task init functions, switch all relays to off state
  pinMode(RELAY_1, OUTPUT); 
  digitalWrite(RELAY_1, 0);
  Rel1_is_on = false;
  while(1)
  {
    if(Req_Rel1_on)                                     // relay off and on request?
    {
      analogWrite(RELAY_1, REL_PWM_HIGH_VALUE);         // Relay group 1 on now (high PWM)
      vTaskDelay(REL_PWM_HIGH_TIME/portTICK_PERIOD_MS); // give time to activate relay
      analogWrite(RELAY_1, REL_PWM_LOW_VALUE);          // go back to reduced PWM
      Rel1_is_on  = true;                               // store new state
      Req_Rel1_on = false;                              // switch on request is handled now
    }
    if(Req_Rel1_off)                                    // relay on and off request?
    {
      analogWrite(RELAY_1, 0);                          // PWM = zero => relay is off
      vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);  // give time to release the coil
      Rel1_is_on   = false;                             // store new state
      Req_Rel1_off = false;                             // switch off request is handled now
    }
    vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);    // ensure task is not blocking
  }  
}

// ----------------------------------------------------------------------------
void MyRelay_2_Task(void *parameter)
{
  // task init functions, switch all relays to off state
  pinMode(RELAY_2, OUTPUT); 
  digitalWrite(RELAY_2, 0);
  while(1)
  {
    if(Req_Rel2_on)                                     // relay off and on request?
    {
      analogWrite(RELAY_2, REL_PWM_HIGH_VALUE);         // Relay group 1 on now (high PWM)
      vTaskDelay(REL_PWM_HIGH_TIME/portTICK_PERIOD_MS); // give time to activate relay
      analogWrite(RELAY_2, REL_PWM_LOW_VALUE);          // go back to reduced PWM
      Rel2_is_on  = true;                               // store new state
      Req_Rel2_on = false;                              // switch on request is handled now
    }
    if(Req_Rel2_off)                                    // relay on and off request?
    {
      analogWrite(RELAY_2, 0);                          // PWM = zero => relay is off
      vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);  // give time to release the coil
      Rel2_is_on   = false;                             // store new state
      Req_Rel2_off = false;                             // switch off request is handled now
    }
    vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);    // ensure task is not blocking
  }  
}

// ----------------------------------------------------------------------------
void MyRelay_3_Task(void *parameter)
{
  // task init functions, switch all relays to off state
  pinMode(RELAY_3, OUTPUT); 
  digitalWrite(RELAY_3, 0);
  while(1)
  {
    if(Req_Rel3_on)                                     // relay off and on request?
    {
      analogWrite(RELAY_3, REL_PWM_HIGH_VALUE);         // Relay group 1 on now (high PWM)
      vTaskDelay(REL_PWM_HIGH_TIME/portTICK_PERIOD_MS); // give time to activate relay
      analogWrite(RELAY_3, REL_PWM_LOW_VALUE);          // go back to reduced PWM
      Rel3_is_on  = true;                               // store new state
      Req_Rel3_on = false;                              // switch on request is handled now
    }
    if(Req_Rel3_off)                                    // relay on and off request?
    {
      analogWrite(RELAY_3, 0);                          // PWM = zero => relay is off
      vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);  // give time to release the coil
      Rel3_is_on   = false;                             // store new state
      Req_Rel3_off = false;                             // switch off request is handled now
    }
    vTaskDelay(REL_MIN_OFF_TIME/portTICK_PERIOD_MS);    // ensure task is not blocking
  }  
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// command interface to the relay tasks
// ----------------------------------------------------------------------------
void MyRelays_Function(RCommands Command)
{
  switch (Command)
  {
    case Cmd_Relay1_On:   if(REL_1_IS_OFF) Req_Rel1_on  = true; break;
    case Cmd_Relay1_Off:  if(REL_1_IS_ON)  Req_Rel1_off = true; break;       
    case Cmd_Relay2_On:   if(REL_2_IS_OFF) Req_Rel2_on  = true; break;
    case Cmd_Relay2_Off:  if(REL_2_IS_ON)  Req_Rel2_off = true; break;
    case Cmd_Relay3_On:   if(REL_3_IS_OFF) Req_Rel3_on  = true; break;
    case Cmd_Relay3_Off:  if(REL_3_IS_ON)  Req_Rel3_off = true; break;
  }
}

// ----------------------------------------------------------------------------
// end of the relay control task module
// ----------------------------------------------------------------------------
