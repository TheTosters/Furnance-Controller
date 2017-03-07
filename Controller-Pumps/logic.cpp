#include <Arduino.h>
#include "logic.h"
#include "temperature.h"

LogicConfig logicConfig;
bool wantHeaters;
bool wantFeederRun;
bool startedMode;
static bool CWIsHeating;
static bool COIsHeating;
static int32_t cyclicDelay;
static int16_t cyclicRun;
static bool isCyclicRun;
static unsigned long lastMillis;

void beginLogic() {
  pinMode(PUMP_CO, OUTPUT);
  pinMode(PUMP_CW, OUTPUT);
  pinMode(PUMP_HEATERS, OUTPUT);
  CWIsHeating = false;
  COIsHeating = false;
  cyclicDelay = 0;
  isCyclicRun = false;
  lastMillis = millis();
}

void updateCW() {
  const float temp = temperature[TEMP_CW];
  if ( CWIsHeating ) {
    if (temp < logicConfig.maxCWTemp) {
      if (temperature[TEMP_CW] < temperature[TEMP_CO]) {
        pumpRunning(PUMP_CW, true);
      } else {
        pumpRunning(PUMP_CW, false);
      }
      
    } else {
      CWIsHeating = false;
      pumpRunning(PUMP_CW, false);
    }
    
  } else {
    if (temp < logicConfig.minCWTemp) {
      CWIsHeating = true;
    }
  }
}

void updateCO() {
  const float temp = temperature[TEMP_CO];
  if ( COIsHeating ) {
    if (temp < logicConfig.maxCOTemp) {
      wantFeederRun = true;
      
    } else {
      COIsHeating = false;
      wantFeederRun = false;
    }
    
  } else {
    if (temp < logicConfig.minCOTemp) {
      COIsHeating = true;
    }
  }
}

bool updateHeaters() {
  if (wantHeaters == false) {
    return false;
  }

  bool state = temperature[TEMP_CO] > logicConfig.minCOTemp;  
  return state;
}

bool updateCyclic() {
  unsigned long ms = millis();
  unsigned long delta = lastMillis - ms;
  lastMillis = ms;

  bool coAndHeaterRun;
  
  if (isCyclicRun) {
    //we are in run
    cyclicRun -= delta;
    if (cyclicRun <= 0) {
      isCyclicRun = false;
      cyclicDelay = logicConfig.heaterPumpCyclicRunDelay;
      cyclicRun = logicConfig.heaterPumpCyclicRunDuration;
      coAndHeaterRun = true;
      
    } else {
      coAndHeaterRun = false;
    }
    
  } else {
    //we waiting for run
    cyclicDelay -= delta;
    if (cyclicDelay < 0) {
      isCyclicRun = true;
      coAndHeaterRun = true;
      
    } else {
      coAndHeaterRun = false;
    }
  }

  return coAndHeaterRun;
}

void updateLogic() {
  updateCO();
  updateCW();
  bool heaters = updateHeaters();
  
  //cyclic pump run
  if (heaters == true) {
    //reset cyclic data
    cyclicDelay = logicConfig.heaterPumpCyclicRunDelay;
    cyclicRun = logicConfig.heaterPumpCyclicRunDuration;
    isCyclicRun = false;

  } else {
    //handle cyclic run
    heaters |= updateCyclic();
  }

  //control pumps with final check
  if (heaters && (temperature[TEMP_CO] > logicConfig.minCOTemp)) {
    pumpRunning(PUMP_CO, true);
    pumpRunning(PUMP_HEATERS, true);
    
  } else {
    pumpRunning(PUMP_CO, false);
    pumpRunning(PUMP_HEATERS, false);
  }
}

void pumpRunning(uint8_t pompId, bool isOn) {
  digitalWrite(pompId, isOn ? HIGH : LOW);
}

