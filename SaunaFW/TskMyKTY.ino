// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs temperature measurement for sauna sensor KTY 35 and room sensor KTY 81
// ----------------------------------------------------------------------------

#define KTY_TIME  50                    // one cycle = 50ms
#define KTY_LOOPS 8                     // number of integration rounds

// -------- Hardware ----------
const float Vref      = 3.3;            // Vref = supply voltage ADC
const float ADCFactor = Vref/4095.0;    // Korrekturfaktor digits => Spannung 
const float Pullup    = 2000.0;         // Pull-Up-Widerstand in Ohm

// -------- KTY (2 kΩ @ 25°C) ----------
// Grundwiderstand + Polynomkoeffizienten (Infineon KTY-Normkurve, typisch)
const float R25 = 2000.0;               // *** angepasst ***
const float A = 7.88e-3;                // 1/K
const float B = 1.94e-5;                // 1/K^2
const float C = 3.42e-8;                // 1/K^3

float U_KTY35, U_KTY81, R_KTY35, R_KTY81;
int   KTY_LoopCount = 0;

// ----------------------------------------------------------------------------
// R(T): Widerstand aus Temperatur (Polynom, T in °C)
float R_from_T(float T) 
{
  float x = T - 25.0f;
  return R25 * (1.0f + A*x + B*x*x + C*x*x*x);
}

// ----------------------------------------------------------------------------
// dR/dT: Ableitung für Newton
float dR_dT(float T) 
{
  float x = T - 25.0f;
  return R25 * (A + 2.0f*B*x + 3.0f*C*x*x);
}

// ----------------------------------------------------------------------------
// T(R): Temperatur aus Widerstand via Newton-Iteration
float T_from_R(float R)
{
  float T = 60.0f;  // Startwert im Zielbereich 20..100°C
  for (int i = 0; i < 4; ++i) {
    float f  = R_from_T(T) - R;
    float df = dR_dT(T);
    T -= f / df;
  }
  return (int) T;
}

// ----------------------------------------------------------------------------
//    Die Werte stimmen noch nicht!!!
//    Spannung wird vermutlich falsch gemessen
//    Widerstand auf jeden Fall, und damit natürlich auch die Temperature
// ----------------------------------------------------------------------------
void MyKTY_Task(void *parameter)
{
  // task setup part
  analogReadResolution(12);       // 0..4095
  analogSetAttenuation(ADC_11db); // Messbereich bis ~3.1 V

  while(1)
  {
    // init all variables

    // 1) read first ADC xtimes and calculate voltage
    KTY_LoopCount = 0;                          // prepare first measurment loop..
    U_KTY35       = 0;                          // clear voltage summation variable
    while (KTY_LOOPS >= KTY_LoopCount)           // integration loop 
    {
      U_KTY35 += analogReadMilliVolts(KTY_35);  // one integration operation
      KTY_LoopCount++;                          // loop counter 
      vTaskDelay(KTY_TIME/portTICK_PERIOD_MS);  // 50 ms pause
    }
    U_KTY35 /= (1000.0 * KTY_LOOPS);            // calculate "real" voltage from integral
    //Serial.print("UKTY 35: "); Serial.println(U_KTY35);

    // 2) read second ADC xtimes and calculate voltage
    KTY_LoopCount = 0;                          // prepare first measurment loop..
    U_KTY81       = 0;                          // clear voltage summation variable
    while (KTY_LOOPS >= KTY_LoopCount)           // integration loop 
    {
      U_KTY81 += analogReadMilliVolts(KTY_81);  // one integration operation
      KTY_LoopCount++;                          // loop counter 
      vTaskDelay(KTY_TIME/portTICK_PERIOD_MS);  // 50 ms pause
    }
    U_KTY81 /= (1000.0 * KTY_LOOPS);            // calculate "real" voltage from integral
    //Serial.print("UKTY 81: "); Serial.println(U_KTY81);

    // 3) calculate sauna temperature
    R_KTY35=Pullup*(U_KTY35/(Vref-U_KTY35));    // Sensorwiderstand aus Spannungsteiler berechnen  
    //Serial.print("Widerstand KTY35:"); Serial.println(R_KTY35);
    SaunaTemperature = T_from_R(R_KTY35);       // Temperatur aus Widerstand berechnen
    vTaskDelay(KTY_TIME/portTICK_PERIOD_MS);    // 50 ms pause

    // 4) calculate room temperature
    R_KTY81=Pullup*(U_KTY81/(Vref-U_KTY81));    // Sensorwiderstand aus Spannungsteiler berechnen
    //Serial.print("Widerstand KTY81:"); Serial.println(R_KTY81);
    RoomTemperature = T_from_R(R_KTY81);        // Temperatur aus Widerstand berechnen
    vTaskDelay(KTY_TIME/portTICK_PERIOD_MS);    // 50 ms pause

    flag_NewTemperatureAvailable = true;
  }
}

// ----------------------------------------------------------------------------
// end of the temperature measurement module
// ----------------------------------------------------------------------------
