// Program odczytuje temperaturę kilku czujników
/*
 * How to connect:
 * D2 -> One Wire DS1820 (define ONEWIRE_PIN at temperature.cpp)
 * D3 -> Co PUMP (define PUMP_CO at logic.h)
 * D4 -> CO-CW Pump (define PUMP_CW at logic.h)
 * D5 -> Heaters Pump (define PUMP_HEATERS at logic.h)
 */

#include "temperature.h"
#include "logic.h"
#include "Communication.h"

//#define DEBUG_MAIN 1
//to debug communication enable DEBUG_COMMUNICATION in communication.cpp

void setup() {
  while(!Serial);
  Serial.begin(57600);

  beginTemperatures();
  beginLogic();
  initCommunication();
}

void emergencyMode() {
  if (temperature[TEMP_CO] < -273) {
    //C.O. sensor is broken. Run pumps to prevent overheat
    pumpRunning(PUMP_CO, true);
    pumpRunning(PUMP_HEATERS, true);
  }
}

void loop() {
/*
  if (sensorsError == true) {
#ifdef DEBUG_MAIN
  Serial.println("Hardware error, can't work!");
#endif
    //temperature sensors are not working as expected.
    emergencyMode();
    return;
  } */
  
  updateTemperatures();
  updateLogic();
  
#ifdef DEBUG_MAIN
  Serial.print("Temp C.O.:");
  Serial.print(temperature[TEMP_CO]);
  Serial.println(" st.C");

  Serial.print("Temp C.W.:");
  Serial.print(temperature[TEMP_CW]);
  Serial.println(" st.C");
#endif
}
