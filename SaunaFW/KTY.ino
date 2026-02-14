// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs temperature measurement for sauna sensor KTY 35 and room sensor KTY 81
// ----------------------------------------------------------------------------

// -------- Hardware ----------
const float Vref  = 3.3;                // Versorgung / ADC-Top
const float ADCFactor = Vref/4095.0f;   // Korrekturfaktor digits => Spannung 
const float Pullup = 2000.0;            // Pull-Up-Widerstand in Ohm

// -------- KTY (2 kΩ @ 25°C) ----------
// Grundwiderstand + Polynomkoeffizienten (Infineon KTY-Normkurve, typisch)
const float R25 = 2000.0;               // *** angepasst ***
const float A = 7.88e-3;                // 1/K
const float B = 1.94e-5;                // 1/K^2
const float C = 3.42e-8;                // 1/K^3

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
int T_from_R(float R)
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
void MyKTY_Init() 
{
  analogReadResolution(12);     // 0..4095
  analogSetAttenuation(ADC_11db); // Messbereich bis ~3.6 V
}

// ----------------------------------------------------------------------------
void MyKTY_Function(void)
{
  float u;
  int   SaunaOld, RoomOld;

  // prepare edge detection
  SaunaOld = SaunaTemperature;
  RoomOld  = RoomTemperature;

  // 1) ADC lesen, Spannung berechnen
  u = (float)analogRead(KTY_35) * ADCFactor;
  
  // 2) Sensorwiderstand aus Spannungsteiler berechnen
  //    Uadc = Vref * Rkty / (Rkty + Rpullup)  ->  Rkty = Rpullup * U / (Vref - U)
  // und direkt daraus die Temperatur bestimmen
  SaunaTemperature = T_from_R(Pullup * (u / (Vref - u)));

  // 1) ADC lesen, Spannung berechnen
  u = (float)analogRead(KTY_81) * ADCFactor;
  
  // 2) Sensorwiderstand aus Spannungsteiler berechnen
  //    Uadc = Vref * Rkty / (Rkty + Rpullup)  ->  Rkty = Rpullup * U / (Vref - U)
  // und direkt daraus die Temperatur bestimmen
  RoomTemperature = T_from_R(Pullup * (u / (Vref - u)));

  // Check for changes
  if (SaunaOld != SaunaTemperature) flag_SaunaTemperatureChanged = true;
  if (RoomOld  != RoomTemperature)  flag_RoomTemperatureChanged = true;
  SaunaOld = SaunaTemperature;
  RoomOld  = RoomTemperature;
}

// ----------------------------------------------------------------------------
// end of the temperature measurement module
// ----------------------------------------------------------------------------
