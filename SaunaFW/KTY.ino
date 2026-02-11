// ----------------------------------------------------------------------------
// My Sauna control GUI handler module 
// Performs temperature measurement for sauna sensor KTY 35 and room sensor KTY 81
// ----------------------------------------------------------------------------

// ab hier noch der Original KI Code
// Funktionen müssen noch an das System angepasst werden
// es fehlt auch noch ein Kanalo für den KTY-81

// -------- Hardware ----------
const int   adcPin = 34;      // ADC-Eingang (GPIO34 ist nur Eingang -> ideal)
const float Vref  = 3.3;      // Versorgung / ADC-Top
const float pullup = 2000.0;  // Pull-Up-Widerstand in Ohm

// -------- KTY (2 kΩ @ 25°C) ----------
const float R25 = 2000.0;     // *** angepasst ***

// Polynomkoeffizienten (Infineon KTY-Normkurve, typisch)
const float A = 7.88e-3;      // 1/K
const float B = 1.94e-5;      // 1/K^2
const float C = 3.42e-8;      // 1/K^3

void KTY_Init() 
{
  analogReadResolution(12);     // 0..4095
  analogSetAttenuation(ADC_11db); // Messbereich bis ~3.6 V
}

// R(T): Widerstand aus Temperatur (Polynom, T in °C)
float R_from_T(float T) 
{
  float x = T - 25.0f;
  return R25 * (1.0f + A*x + B*x*x + C*x*x*x);
}

// dR/dT: Ableitung für Newton
float dR_dT(float T) 
{
  float x = T - 25.0f;
  return R25 * (A + 2.0f*B*x + 3.0f*C*x*x);
}

// T(R): Temperatur aus Widerstand via Newton-Iteration
float T_from_R(float R)
{
  float T = 60.0f;  // Startwert im Zielbereich 20..100°C
  for (int i = 0; i < 4; ++i) {
    float f  = R_from_T(T) - R;
    float df = dR_dT(T);
    T -= f / df;
  }
  return T;
}

void KTY_Function(void)
{
  // 1) ADC lesen
  int   adc = analogRead(adcPin);
  float u   = adc * (Vref / 4095.0f);

  // 2) Sensorwiderstand aus Spannungsteiler berechnen
  //    Uadc = Vref * Rkty / (Rkty + Rpullup)  ->  Rkty = Rpullup * U / (Vref - U)
  float Rkty = pullup * (u / (Vref - u));

  // 3) Temperatur bestimmen
  float T = T_from_R(Rkty);

  // Ausgabe
  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print("  U=");
  Serial.print(u, 3);
  Serial.print(" V  R=");
  Serial.print(Rkty, 1);
  Serial.print(" Ohm  T=");
  Serial.print(T, 2);
  Serial.println(" °C");

  delay(1000);
}