// Program odczytuje temperaturę kilku czujników

#include "temperature.h"
#include "logic.h"

#define DUMP_TO_SERIAL 1

void setup() {
#ifdef DUMP_TO_SERIAL
  while(!Serial);
  Serial.begin(9600);
#endif

  beginTemperatures();
  beginLogic();
}

void emergencyMode() {
  if (temperature[TEMP_CO] < -273) {
    //C.O. sensor is broken. Run pumps to prevent overheat
    pumpRunning(PUMP_CO, true);
    pumpRunning(PUMP_HEATERS, true);
  }
}

void loop() {
  if (sensorsError == true) {
#ifdef DUMP_TO_SERIAL
  Serial.println("Hardware error, can't work!");
#endif
    //temperature sensors are not working as expected.
    emergencyMode();
    return;
  }
  
  updateTemperatures();
  updateLogic();
  
#ifdef DUMP_TO_SERIAL
  Serial.print("Temp C.O.:");
  Serial.print(temperature[TEMP_CO]);
  Serial.println(" st.C");

  Serial.print("Temp C.W.:");
  Serial.print(temperature[TEMP_CW]);
  Serial.println(" st.C");
#endif
}
