// ----------------------------------------------------------------------------
// SaunaFW definitions, global variables
// included by SaunaFW.ino (projekt main source file)
// ----------------------------------------------------------------------------
//  Arduino ESP32 pin definitions
// ----------------------------------------------------------------------------
#define I2C_SDA             21      // default SDA for ESP32
#define I2C_SCL             22      // ditto SCL

#define ENC_CLK             5       // Signal A interrupt
#define ENC_DT              17      // Signal B to interrupt
#define ENC_SW              18      // Signal button, to interrupt
#define ENC_VCC             16      // control vcc of the encoder

#define RELAY_1             26      // 3.3V signal for relay group #1 (safety)
#define RELAY_2             27      // ditto relay group #2 (regulation)
#define RELAY_3             14      // ditto relay #3 (light)

#define INP_DOOR            15      // door contact, low if door closed
#define INP_LIGHT           12      // light on/off button on GPIO12
#define INP_MODE            13      // mode: on, standby ....

#define DEBUG_1             19      // Debug pin #1

#define KTY_35              4       // oven sensor = KTY_35
#define KTY_81              2       // room sensor = KTY_81

#define BUZZER              23      // buzzer output
// ----------------------------------------------------------------------------
// Some commcon constants, magic nubers, ...
// ----------------------------------------------------------------------------
#define I2C_CLOCK           400000  // higher speed does not reduce transfer timem

#define Temp_Min            50      // minimum temparature possible to select
#define Temp_Max            100     // maximum temperature possible to select
#define Temp_Def            80      // default temperature after powre on
#define Temp_Store          3000    // time for button press to store new default

#define SSID_Len            33u     // standard = 32 chars + #0
#define PWD_Len             63u     // standard = 63 chars + #0

#define LCD_ADR             0x27    // i²C default address, depends on chip (!)
#define LCD_WIDTH           20      // 20 characters per line
#define LCD_LINES           4       // 4 lines on display

#define REL_PWM1_VALUE    	230     // 90%* 2,55 higher PWM value in digits
#define REL_PWM1_TIME   	  2000    // time for this first phase
#define REL_PWM2_VALUE  	  191     // 75% * 255 final PWM value for hold in digits

#define SET_DEBUG_1(x)		  digitalWrite(DEBUG_1, (x))

// These command are used a lot; 
// to make code more compact macros are defined for that
#define SP                  Serial.print
#define SPLF                Serial.println
#define SW                  Serial.write

// ----------------------------------------------------------------------------
// Global variables and definitions
// 1. configuration related
// ----------------------------------------------------------------------------
typedef struct                  // configuration data definition
{
  uint16_t  MagicNumber;        // alays 0xdead
  uint8_t   WlanSsid[SSID_Len]; // user defined strings
  uint8_t   WlanPwd[PWD_Len];   // 
  uint16_t  NtpPeriod;          // update period in ms
  uint16_t  NtpOffset;          // time zone offset for NTP reading
  uint8_t   STimeMode;          // summer time 0 = off, 1 = on, 2 = auto 
  uint16_t  DefTemperature;     // sauna target temperature after power on
  uint16_t  Checksum;           // to be calculated before store
} Config_s;

const Config_s DefaultConfig =  // used on empty or invalid EEPROM
{
  0xdead,                       // magic number
  "YOUR_SSID",                  // WLAN SSID home WLAN
  "YOUR_Password",              // WLAN PWD 
  60000,                        // NTP update interval ever 60s
  3600,                         // 3600 for european standard time, Berlin
  0,                            // no summer time
  80,                           // 80°C default temperature
  0x00000000                    // must be calculated before store
};  

enum                            
{
  EE_NotRead,                   // init, EEPROM not read
  EE_Empty,                     // EEPROM seems to be empty
  EE_Invalid,                   // data found; checksum invalid
  EE_Valid                      // ;-)
} ConfigState = EE_NotRead;     // global config state

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
#define NTP_PERIOD      ConfigUnion.ActConfig.NtpPeriod
#define NTP_OFFSET      ConfigUnion.ActConfig.NtpOffset
#define STIME_MODE      ConfigUnion.ActConfig.STimeMode
#define DEF_TEMPERATURE ConfigUnion.ActConfig.DefTemperature
#define CHECKSUM        ConfigUnion.ActConfig.Checksum
#define WLAN_CONNECTED  WLANState == WLAN_Connected
#define NTP_SERVER      "pool.ntp.org"

// ----------------------------------------------------------------------------
// Global variables and definitions
// 2. temperature and time related
// ----------------------------------------------------------------------------
float RoomTemperature  = 0.0;                 // current room temperature
float SaunaTemperature = 0.0;                 // current sauna temperature
int  TargetTemperature = DEF_TEMPERATURE;     // selected sauna target temperature

// flags for changed
bool flag_NewTemperatureAvailable   = false;  // one flag set by KTY module
bool flag_TargetTemperatureChanged  = false;  // two flags set by encoder module
bool flag_DefaultTemperatureChanged = false; 

struct
{
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
} MyNTPTime;

// ----------------------------------------------------------------------------
// Global variables and definitions
// 3. Hardware and system related
// ----------------------------------------------------------------------------
LiquidCrystal_I2C           myLCD(LCD_ADR, LCD_WIDTH, LCD_LINES);
RotaryEncoder               myEnc(ENC_CLK, ENC_DT, ENC_SW, ENC_VCC);

// ----------------------------------------------------------------------------
// Global variables and definitions
// 4. Relaiy application module related
// ----------------------------------------------------------------------------

// relay interface from module to the application
typedef enum 		
{
  Cmd_None,
  Cmd_Relay1_On,				    // standard on command relay group #1 (safety)
  Cmd_Relay1_Off,				    // ditto off
  Cmd_Relay2_On,				    // standard on command relay group #2 (regulation)
  Cmd_Relay2_Off,				    // ditto off
  Cmd_Relay3_On,            // standard on command relay #3 (light)
  Cmd_Relay3_Off            // ditto off
} RCommands;                // commands handled by the relay command interface

bool Rel1_is_on = false;    // actual releay state
bool Rel2_is_on = false;
bool Rel3_is_on = false;

#define REL_1_IS_ON   (false != Rel1_is_on)
#define REL_1_IS_OFF  (false == Rel1_is_on)
#define REL_2_IS_ON   (false != Rel2_is_on)
#define REL_2_IS_OFF  (false == Rel2_is_on)
#define REL_3_IS_ON   (false != Rel3_is_on)
#define REL_3_IS_OFF  (false == Rel3_is_on)

// ----------------------------------------------------------------------------
// Global variables and definitions
// 5. Debouncer and input signals relatec
// ----------------------------------------------------------------------------

bool flag_mode_pressed   = false;  // flags to be set by the edge generator
bool flag_mode_released  = false;  //
bool flag_light_pressed  = false;  //
bool flag_light_released = false;  //
bool flag_door_opened    = false;  //
bool flag_door_closed    = false;  //

#define MODE_IS_PRESSED     (0 != (deb_dat & 1)) // for static signal check
#define LIGHT_IS_PRESSED    (0 != (deb_dat & 2))
#define DOOR_IS_CLOSED      (0 != (deb_dat & 4))

// ----------------------------------------------------------------------------
// Global variables and definitions
// 6. WLAN and NTP related
// ----------------------------------------------------------------------------
typedef enum                            
{
  WLAN_NotConnected,            // not connected on startup
  WLAN_Connected                // ;-)
} t_WLANState;
t_WLANState WLANState = WLAN_NotConnected;      // global WLAN state
t_WLANState LastWLANState = WLAN_NotConnected;  // flag to detect changed

WiFiUDP     ntpUDP;
NTPClient   timeClient(ntpUDP, NTP_SERVER, (uint16_t)NTP_OFFSET, (uint16_t)NTP_PERIOD); 

bool        NTP_Online = false;   // flag to signalize time handler NTP is online onr not
bool        NTP_Updated = false;  // dito, new data available
uint8_t     NTP_TimeOutCtr = 0;   // counter to detect online state

// ----------------------------------------------------------------------------
// Global variables and definitions
// 9. Others
// ----------------------------------------------------------------------------
// used for background serial handling 
#define STRG_X      24  // restart system
#define STRG_W      23  // switch WLAN (for test purposes)
#define STRG_C      03  // call configuration menu


// ----------------------------------------------------------------------------
//  end of include file
// ----------------------------------------------------------------------------

