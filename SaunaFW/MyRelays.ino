// ----------------------------------------------------------------------------
// My Sauna control Relay handler module 
// Performs all actions with user
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Relay 1 and 2 control FSM
// called from 5ms Task
// ----------------------------------------------------------------------------
void Relay_FSM(void)
{
  switch (Relay1and2FSM)
  {
    case RelayInit:   // temporary state only once after init
    break;
    case Relay_Off:   // stable, relay(s) is/are off
    break;
    case Relay_PWM1:  // first phase: high PWM    (to move the system)
    break;
    case Relay_PWM2:  // final phase: medium PWM  (to hold the system)
    break;
  }
  
  switch (Relay3FSM)
  {
    case RelayInit:   // temporary state only once after init
    break;
    case Relay_Off:   // stable, relay(s) is/are off
    break;
    case Relay_PWM1:  // first phase: high PWM    (to move the system)
    break;
    case Relay_PWM2:  // final phase: medium PWM  (to hold the system)
    break;
  }

  // ... noch mit leben füllen


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
    break;
    case RelaysLx_Off:
    break;
    case RelayLight_On:
    break;
    case RelayLight_Off:
    break;
  }
}

// ----------------------------------------------------------------------------
