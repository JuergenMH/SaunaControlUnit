// ----------------------------------------------------------------------------
// SaunaFW definitions, global variables
// included by SaunaFW.ino (projekt main source file)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Arduino pin definitions
// ----------------------------------------------------------------------------
#define I2C_SDA     21      // default SDA for ESP32
#define I2C_SCL     22      // ditto SCL

#define ENC_CLK     5       // Signal A interrupt
#define ENC_DT      17      // Signal B to interrupt
#define ENC_SW      18      // Signal button, to interrupt
#define ENC_VCC     16      // control vcc of the encoder

#define RELAY_1     26      // 3.3V signal for relay group #1 (safety)
#define RELAY_2     27      // ditto relay group #2 (regulation)
#define RELAY_3     14      // ditto relay #3 (light)

#define INP_DOOR    15      // door contact, low if door closed
#define INP_LIGHT   12      // light on/off button on GPIO12
#define INP_MODE    13      // mode: on, standby ....

#define DEBUG_1		19		// Debug pin #1
#define DEBUG_2		23		// Debug Pin #2

// ----------------------------------------------------------------------------
// Some commcon constants, magic nubers, ...
// ----------------------------------------------------------------------------
#define Temp_Min    50      // minimum temparature possible to select
#define Temp_Max    100     // maximum temperature possible to select
#define Temp_Def    80      // default temperature after powre on
#define Temp_Store  3000    // time for button press to store new default

#define SSID_Len    33u     // standard = 32 chars + #0
#define PWD_Len     63u     // standard = 63 chars + #0

#define LCD_ADR     0x27    // i²C default address, depends on chip (!)
#define LCD_WIDTH   20      // 20 characters per line
#define LCD_LINES   4       // 4 lines on display

#define REL_PWM1_VALUE  	230     // 90%* 2,55 higher PWM value in digits
#define REL_PWM1_TIME   	2000    // time for this first phase
#define REL_PWM2_VALUE  	191     // 75% * 255 final PWM value for hold in digits

// some function like macros
#define LOAD_REL12_TIMER(x) SW_Timer_1=x
#define LOAD_REL3_TIMER(x)  SW_Timer_2=x
#define REL12_TIMER_ELAPSED SW_Timer_1 == 0
#define REL3_TIMER_ELAPSED  SW_Timer_2 == 0

#define SET_DEBUG_1(x)		digitalWrite(DEBUG_1, (x))
#define SET_DEBUG_2(x)		digitalWrite(DEBUG_2, (x))

// timing measurement related macros
//#define MEASURE_TASK_1
//#define MEASURE_TASK_51		// IO drive + Light handler
//#define MEASURE_TASK_52	  // Temperature measurement
//#define MEASURE_TASK_53
//#define MEASURE_TASK_54
//#define MEASURE_TASK_55
//#define MEASURE_TASK_BKG

// ----------------------------------------------------------------------------
// Global variables and definitions
// 1. temperature related
// ----------------------------------------------------------------------------
unsigned int RoomTemperature    = 0;          // current room temperature
unsigned int SaunaTemperature   = 0;          // current sauna temperature
unsigned int TargetTemperature  = Temp_Def;   // selected sauna target temperature
unsigned int DefaultTemperature = Temp_Def;   // (new) default temperature to be stored 
bool TargetTemperatureChanged   = false;      // flag to the main function, target changed
bool DefaultTemperatureChanged  = false;      // ditto but for default temperature

// ----------------------------------------------------------------------------
// Global variables and definitions
// 2. Hardware and system related
// ----------------------------------------------------------------------------
unsigned int        SW_Timer_1 = 0;
unsigned int        SW_Timer_2 = 0;
unsigned int        SW_Timer_3 = 0;

LiquidCrystal_I2C   myLCD(LCD_ADR, LCD_WIDTH, LCD_LINES);
RotaryEncoder       myEnc(ENC_CLK, ENC_DT, ENC_SW, ENC_VCC);

// ----------------------------------------------------------------------------
// Global variables and definitions
// 3. Relaiy application module related
// ----------------------------------------------------------------------------
#define REL_PWM_HIGH_VALUE  205     // 90% * 2,55 higher PWM value digits
#define REL_PWM_LOW_VALUE   127     // 50% * 2,55 final PWM value in digits
#define REL_PWM_HIGH_TIME   50      // time for high PWM in ms

#define LOAD_REL1_TIMER(x)  SW_Timer_1=x
#define LOAD_REL2_TIMER(x)  SW_Timer_2=x
#define LOAD_REL3_TIMER(x)  SW_Timer_3=x

#define REL1_TIMER_ELAPSED  SW_Timer_1 == 0
#define REL2_TIMER_ELAPSED  SW_Timer_2 == 0
#define REL3_TIMER_ELAPSED  SW_Timer_3 == 0

// relay interface from module to the application
typedef enum 		
{
  Cmd_None,
  Cmd_Relay1_On,				    // standard on command relay group #1 (safety)
  Cmd_Relay1_Off,				    // ditto off
  Cmd_Relay2_On,				    // standard on command relay group #2 (regulation)
  Cmd_Relay2_Off,				    // ditto off
  //Cmd_Relay1_and_2_On,      // fire both relays in a sequence
  //Cmd_Relay1_and_2_Off,     // ditto both off
  Cmd_Relay3_On,            // standard on command relay #3 (light)
  Cmd_Relay3_Off            // ditto off
} RCommands;                // commands handled by the relay command interface

typedef enum                // used for all relay FSMs
{
  FSM_Relay_Init,           // temporary state only once after init
  FSM_Relay_Off,            // stable state:        relay is off
  FSM_Relay_PWM,            // intermediate state:  relay on with high PWM
  FSM_Relay_On              // stable state:        relay on with lower PWM
} Relay_FSM;

Relay_FSM  Relay1_FSM = FSM_Relay_Init;   // control relay group #1 (safety)
Relay_FSM  Relay2_FSM = FSM_Relay_Init;   // control relay gropu #2 (regulation)
Relay_FSM  Relay3_FSM = FSM_Relay_Init;   // control relay #3		(light)

// interface between relay FSM and the module command interface
bool Req_Rel1_on  = false;
bool Req_Rel1_off = false;
bool Req_Rel2_on  = false;
bool Req_Rel2_off = false;
bool Req_Rel3_on  = false;
bool Req_Rel3_off = false;

// ----------------------------------------------------------------------------
// Global variables and definitions
// 4. Debouncer and input signals relatec
// ----------------------------------------------------------------------------
uint8_t bnc_cnt = 0;		      // counter for debounce cycles (pointer to array)
uint8_t bnc_dat_array[3];	    // bouncing data input array for three samples
uint8_t deb_dat_count = 0;	  // counter for debouncer data valid decision
uint8_t bnc_dat = 0;		      // working variable for debounce algorithm
uint8_t deb_dat = 0;		      // debounced data variable
uint8_t deb_dat_old = 0;      // old variable for edge detection
bool deb_dat_valid = false;   // TRUE if debounced data is valid now

bool flag_mode_pressed   = false;  // flags to be set by the edge generator
bool flag_mode_released  = false;  //
bool flag_light_pressed  = false;  //
bool flag_light_released = false;  //
bool flag_door_opened    = false;  //
bool flag_door_closed    = false;  //

#define PIN_MODE_PRESSED    (false == digitalRead(INP_MODE))
#define PIN_LIGHT_PRESSED   (false == digitalRead(INP_LIGHT))
#define PIN_DOOR_CLOSED     (false == digitalRead(INP_DOOR))

#define MODE_IS_PRESSED     (0 != (deb_dat & 1)) // for static signal check
#define LIGHT_IS_PRESSED    (0 != (deb_dat & 2))
#define DOOR_IS_CLOSED      (0 != (deb_dat & 4))

// ----------------------------------------------------------------------------
// end of the global header file
// ----------------------------------------------------------------------------
// ab hier noch der alte Kram


typedef struct
{
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
} TmEntryS;


// ----------------------------------------------------------------------------
// Global variables and definitions
// 2. configuration related
// ----------------------------------------------------------------------------
typedef struct                  // configuration data definition
{
  uint16_t  MagicNumber;        // alays 0xdead
  uint8_t   WlanEnabled;        // != 0 = enabled
  uint8_t   WlanSsid[SSID_Len]; // user defined strings
  uint8_t   WlanPwd[PWD_Len];   // 
  uint8_t   NtpEnabled;         // != 0 = enabled
  uint16_t  NtpPeriod;          // update period in ms
  uint16_t  NtpOffset;          // time zone offset for NTP reading
  uint8_t   STimeMode;          // summer time 0 = off, 1 = on, 2 = auto 
  uint16_t  Checksum;           // to be calculated before store
} Config_s;

const Config_s DefaultConfig =  // used on empty or invalid EEPROM
{
  0xdead,                       // magic number
  1,                            // WLAN is enabled
  "YourSSID",                   // WLAN SSID home WLAN
  "YourWLANPWD",                // WLAN PWD 
  1,                            // NTP is enabled
  60000,                        // NTP update interval ever 60s
  3600,                         // 3600 for european standard time, Berlin
  0,                            // no summer time
  0x00000000                    // must be calculated before store
};  

enum                            
{
  EE_NotRead,                   // init, EEPROM not read
  EE_Empty,                     // EEPROM seems to be empty
  EE_Invalid,                   // data found; checksum invalid
  EE_Valid                      // ;-)
} ConfigState = EE_NotRead;     // global config state

// ----------------------------------------------------------------------------
// Global variables and definitions
// 3. WLAN and NTP related
// ----------------------------------------------------------------------------
enum                            
{
  WLAN_NotConnected,            // not connected on startup
  WLAN_ConnectionTimeout,       // cnnection failed with timeout
  WLAN_Connected                // ;-)
} WLANState = WLAN_NotConnected;// global WLAN state

const uint16_t ConfigLen     = sizeof DefaultConfig;
const uint16_t MyMagicNumber = 0xdead;

union                           // to hold configuration data set
{
  Config_s  ActConfig;          // "normal" access
  uint8_t   DBytes[ConfigLen];  // ony byte level for EEPROM driver
} ConfigUnion;

// some macros for easy union member access
#define MAGIC_NUMBER    ConfigUnion.ActConfig.MagicNumber
#define WLAN_ENABLED    ConfigUnion.ActConfig.WlanEnabled
#define WLAN_SSID       ConfigUnion.ActConfig.WlanSsid
#define WLAN_PWD        ConfigUnion.ActConfig.WlanPwd
#define NTP_ENABLED     ConfigUnion.ActConfig.NtpEnabled
#define NTP_PERIOD      ConfigUnion.ActConfig.NtpPeriod
#define NTP_OFFSET      ConfigUnion.ActConfig.NtpOffset
#define STIME_MODE      ConfigUnion.ActConfig.STimeMode
#define CHECKSUM        ConfigUnion.ActConfig.Checksum
#define WLAN_CONNECTED  WLANState == WLAN_Connected
#define NTP_SERVER      "pool.ntp.org"

WiFiUDP     ntpUDP;
NTPClient   timeClient(ntpUDP, NTP_SERVER, (uint16_t)NTP_OFFSET, (uint16_t)NTP_PERIOD); 

bool        NTP_Online = false;   // flag to signalize time handler NTP is online onr not
bool        NTP_Updated = false;  // dito, new data available
uint8_t     NTP_TimeOutCtr = 0;   // counter to detect online state

// ----------------------------------------------------------------------------
// Global variables and definitions
// 4. Others
// ----------------------------------------------------------------------------
// used for background serial handling 
#define STRG_X      24  // restart system
#define STRG_W      23  // switch WLAN
#define STRG_C      03  // call configuration menu




// ----------------------------------------------------------------------------
//  end of include file
// ----------------------------------------------------------------------------

