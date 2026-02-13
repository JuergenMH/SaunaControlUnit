// ----------------------------------------------------------------------------
// My Sauna control Relay handler module 
// Performs all relay functions (two relay FSMs)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Relay control FSM
// called from the last 5ms Task
// ----------------------------------------------------------------------------
void MyRelay_FSM(void)
{
  // --------------------------------------------------------------------------
  switch (Relay1_FSM)
  {
    case FSM_Relay_Init:                          // temporary state only once after init
      analogWrite(RELAY_1, 0);                    // relay group 1 off
      Req_Rel1_off = false;                       // in case of unit is used for switch off
      Relay1_FSM   = FSM_Relay_Off;               // enter off FSM state
      break;
      
    case FSM_Relay_Off:                           // stable, relay group #1 is off
      if (Req_Rel1_on)                            // switch on request?
      {
        LOAD_REL1_TIMER(REL_PWM_HIGH_TIME);       // yes, load PWM imer
        analogWrite(RELAY_1, REL_PWM_HIGH_VALUE); // Relay group 1 on now
        Req_Rel1_on = false;                      // request is handled  
        Relay1_FSM = FSM_Relay_PWM;               // next phase: monitor PWM timer
      }
      break;
    
    case FSM_Relay_PWM:                           // first phase: high PWM on #1 to move the system
      if (REL1_TIMER_ELAPSED)                     // first phase elapsed?
      {
        analogWrite(RELAY_1, REL_PWM_LOW_VALUE);  // reduce PWM for relay bank #1
        Relay1_FSM = FSM_Relay_On;                // next state 
      }
      // break;                                   // oink, oink ;-)
    
    case FSM_Relay_On:                            // final phase: medium PWM  (to hold the system)
      if (Req_Rel1_off)                           // switch off request?
      {
        Relay1_FSM = FSM_Relay_Init;              // yes, use init state for switching off
      }
      break;
  }
  // --------------------------------------------------------------------------
  switch (Relay2_FSM)
  {
    case FSM_Relay_Init:                          // temporary state only once after init
      analogWrite(RELAY_2, 0);                    // relay group 2 off
      Req_Rel2_off = false;                       // in case of unit is used for switch off
      Relay2_FSM = FSM_Relay_Off;                 // enter off FSM state
      break;
      
    case FSM_Relay_Off:                           // stable, relay group #2 is off
      if (Req_Rel2_on)                            // switch on request?
      {
        LOAD_REL2_TIMER(REL_PWM_HIGH_TIME);       // yes, load PWM imer
        analogWrite(RELAY_2, REL_PWM_HIGH_VALUE); // Relay group 1 on now
        Req_Rel2_on = false;                      // request is handled  
        Relay2_FSM = FSM_Relay_PWM;               // next phase: monitor PWM timer
      }
      break;
    
    case FSM_Relay_PWM:                           // first phase: high PWM on #1 to move the system
      if (REL2_TIMER_ELAPSED)                     // first phase elapsed?
      {
        analogWrite(RELAY_2, REL_PWM_LOW_VALUE);  // reduce PWM for relay bank #2
        Relay2_FSM = FSM_Relay_On;               	// next state 
      }
      // break;                                   // oink, oink ;-)
    
    case FSM_Relay_On:                            // final phase: medium PWM  (to hold the system)
      if (Req_Rel2_off)                           // switch off request?
      {
        Relay2_FSM = FSM_Relay_Init;              // yes, use init state for switching off

      }
      break;
  }
  // --------------------------------------------------------------------------
	switch (Relay3_FSM)
  {
    case FSM_Relay_Init:                          // temporary state only once after init
      analogWrite(RELAY_3, 0);                    // all off
      Req_Rel3_off = false;                       // in case of unit is used for switch off
      Relay3_FSM  = FSM_Relay_Off;                // enter off FSM state   
      break;

    case FSM_Relay_Off:                           // stable, relay3 is off
      if (Req_Rel3_on)                            // switch on request?
      {
        LOAD_REL3_TIMER(REL_PWM_HIGH_TIME);       // yes, load PWM imer
        analogWrite(RELAY_3, REL_PWM_HIGH_VALUE); // Relay group 1 on now
        Req_Rel3_on = false;                      // request is handled  
        Relay3_FSM  = FSM_Relay_PWM;              // next phase: monitor PWM timer
      }
      break;

   case FSM_Relay_PWM:                            // first phase: high PWM on relay 3 to move the system
     if (REL3_TIMER_ELAPSED)                      // first phase elapsed?
      {
        analogWrite(RELAY_3, REL_PWM_LOW_VALUE);  // reduce PWM for relay 3
        Relay3_FSM = FSM_Relay_On;                // next state 
      }
      // break;                                   // oink, oink ;-)
  
    case FSM_Relay_On:                            // stable relay & low PWM
      if (Req_Rel3_off)                           // switch off request?
      {
        Relay3_FSM = FSM_Relay_Init;              // yes, use init state for switching off
      }
      break;
  }
}

// ----------------------------------------------------------------------------
void Relays_Init(void)
{
  // switch all relays to off state
  pinMode(RELAY_1, OUTPUT); digitalWrite(RELAY_1, 0);
  pinMode(RELAY_2, OUTPUT); digitalWrite(RELAY_2, 0);
  pinMode(RELAY_3, OUTPUT); digitalWrite(RELAY_3, 0);
}

// ----------------------------------------------------------------------------
void Relays_Function(RCommands Command)
{
  switch (Command)
  {
    case Cmd_Relay1_On:
      if (FSM_Relay_Off == Relay1_FSM)      // switch on only in off state
        Req_Rel1_on = true;                 // is off => set the request for the FSM
      break;                                      

    case Cmd_Relay1_Off:
      if (FSM_Relay_Off < Relay1_FSM)       // relay is in a on state?
        Req_Rel1_off = true;                // is on => set the request for the FSM
      break;

    case Cmd_Relay2_On:                     
      if (FSM_Relay_Off == Relay2_FSM)      // switch on only in off state
        Req_Rel2_on = true;                 // is off => set the request for the FSM
      break;                                     

    case Cmd_Relay2_Off:
      if (FSM_Relay_Off < Relay2_FSM)       // relay is in a on state?
        Req_Rel2_off = true;                // is on => set the request for the FSM
      break;

    case Cmd_Relay3_On:
      if (FSM_Relay_Off == Relay3_FSM)      // switch on only in off state
        Req_Rel3_on = true;                 // is on => set the request for the FSM
      break;

    case Cmd_Relay3_Off:
      if (FSM_Relay_Off < Relay3_FSM)       // relay is in a on state?
        Req_Rel3_off = true;                // is on => set the request for the FSM    
      break;
  }
}

// ----------------------------------------------------------------------------
// end of the relay control module
// ----------------------------------------------------------------------------
