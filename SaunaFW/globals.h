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
LiquidCrystal_I2C   myLCD(LCD_ADR, LCD_WIDTH, LCD_LINES);
RotaryEncoder       myEnc(ENC_CLK, ENC_DT, ENC_SW, ENC_VCC);

// ----------------------------------------------------------------------------
// Global variables and definitions
// 3. Relaiy application module related
// ----------------------------------------------------------------------------
typedef enum 
{
  NoCMD,
  RelaysLx_On,
  RelaysLx_Off,
  RelayLight_On,
  RelayLight_Off
} RCommands;

typedef enum      // used for both relay FSMs
{
  RelayInit,      // temporary state only once after init
  Relay_Off,      // stable, relay(s) is/are off
  Relay_PWM1,     // first phase: high PWM    (to move the system)
  Relay_PWM2      // final phase: medium PWM  (to hold the system)
} RelayFSM;

RelayFSM    Relay1and2FSM = RelayInit; // control relay group 1 and 2
RelayFSM    Relay3FSM = RelayInit;     // control the light relay






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

uint8_t             MainSchedule = 0; // arduino style scheduler in main loop


// ----------------------------------------------------------------------------
//  end of include file
// ----------------------------------------------------------------------------

