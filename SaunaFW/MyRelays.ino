// ----------------------------------------------------------------------------
// My Sauna control Relay handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------

#define REL_PWM_HIGH_VALUE  90      // higher PWM value in %
#define REL_PWM_HIGH_TIME   2000    // time for this first phase
#define REL_PWM_LOW_VALUE   75      // final PWM value for hold in % 

#define LOAD_REL12_TIMER(x)   SW_Timer_1=x
#define LOAD_REL3_TIMER(x)    SW_Timer_2=x
#define REL12_TIMER_ELAPSED   SW_Timer_1 == 0
#define REL3_TIMER_ELAPSED    SW_Timer_2 == 0

// ----------------------------------------------------------------------------
// Relay 1 and 2 control FSM
// called from 5ms Task
// ----------------------------------------------------------------------------
void Relay_FSM(void)
{
  switch (Relay1and2FSM)
  {
    case Relay_Init:                              // temporary state only once after init
      analogWrite(RELAY_1, 0);                    // all off
      analogWrite(RELAY_2, 0);
      Relay1and2FSM = Relay_Off;                  // enter off FSM state
      break;
      
    case Relay_Off:                               // stable, relay 1 and 2 are off
      // do nothing in this state
      break;
    
    case Relay_PWM1:                              // first phase: high PWM on #1 to move the system
      if (REL12_TIMER_ELAPSED)                    // first phase elapsed?
      {
        analogWrite(RELAY_1, REL_PWM_LOW_VALUE);  // reduce PWM for relay bank #1
        LOAD_REL12_TIMER(REL_PWM_HIGH_TIME);      // wait again with high power on relay group 2
        analogWrite(RELAY_2, REL_PWM_HIGH_VALUE); // now relay bank 2 is also high powered
        Relay1and2FSM = Relay_PWM2;               // next state 
      }
      break;
    
    case Relay_PWM2:                              // final phase: medium PWM  (to hold the system)
      if (REL12_TIMER_ELAPSED)                    // second phase elapsed?
      {
        analogWrite(RELAY_2, REL_PWM_LOW_VALUE);  // reduce PWM for relay bank #2
        Relay1and2FSM = Relay_On;                 // next state 
      }
      break;
    
    case Relay_On:                                // stable relay 1 and 2 with low PWM
      // do nothing in this state
      break;
  }
  
  switch (Relay3FSM)
  {
    case Relay_Init:                              // temporary state only once after init
      analogWrite(RELAY_3, 0);                    // all off
      Relay3FSM = Relay_Off;                      // enter off FSM state   
      break;

    case Relay_Off:                               // stable, relay3 is off
      // do nothing in this state
      break;

   case Relay_PWM1:                               // first phase: high PWM pn relay 3 to move the system
     if (REL12_TIMER_ELAPSED)                     // first phase elapsed?
      {
        analogWrite(RELAY_3, REL_PWM_LOW_VALUE);  // reduce PWM for relay 3
        Relay3FSM = Relay_On;                     // next state 
      }
      break;
  
    case Relay_On:                                // stable relay § low PWM
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
    case RelaysLx_On:
      LOAD_REL12_TIMER(REL_PWM_HIGH_TIME);        // wait again with high power on relay group 2
      analogWrite(RELAY_1, REL_PWM_HIGH_VALUE);   // first relay bank to high PWM
      Relay1and2FSM = Relay_PWM1;                 // load timer for initial high PWM phase  
      break;                                      // reuse init to switch everything off first

    case RelaysLx_Off:
      LOAD_REL12_TIMER(0);                        // stop SW timer
      Relay1and2FSM = Relay_Init;                 // Use init code for switch off both relays 
      break;

    case RelayLight_On:
      LOAD_REL3_TIMER(REL_PWM_HIGH_TIME);         // wait again with high power on relay group 2
      analogWrite(RELAY_3, REL_PWM_HIGH_VALUE);   // first relay bank to high PWM
      Relay3FSM = Relay_PWM1;                     // load timer for initial high PWM phase  
      break;

    case RelayLight_Off:
      LOAD_REL3_TIMER(0);                         // stop SW timer
      Relay3FSM = Relay_Init;                     // Use init code for switch off relay 3 
      break;
  }
}

// ----------------------------------------------------------------------------
