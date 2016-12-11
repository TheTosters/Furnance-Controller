#include <DallasTemperature.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <stdbool.h>

#include "logic.h"
#include "Timer.h"

WorkConfig workConfig;
OneWire oneWire(TEMP_SENSOR_ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

//furnance is requested to burn, it want fan & feeder
bool workModeRequested;

//master MCU is requesting feeder to be run
bool feederRequested;

//furnance is in WORK mode not in STOP
bool startedMode;

//used by slimakCtrl in fan control interrupt
bool fanEnabled;

int lastFeederTemp;

void initLogic() {
  loadConfig();
  pinMode(FEEDER_PIN, OUTPUT);
  sensors.begin();
  startTimer(FEEDER_TEMP_CHECK_TIMER);
}

void resetConfigToDefault() {
  workConfig.feederRunTime = 8 * 1000;
  workConfig.feederRepeatTime = 30 * 1000;
  workConfig.keepAliveRepeatTime = 30 * 60 * 1000;
  workConfig.keepAliveFanTime = 10 * 1000;
  workConfig.keepAliveFeederTime = 3 * 1000;
  workConfig.feederTempCritical = 40;
}

void saveConfig() {
  int eeAddress = 0;
  EEPROM.put(eeAddress, workConfig);  
}

void loadConfig() {
  int eeAddress = 0;
  EEPROM.get(eeAddress, workConfig);  
}

void updateInWorkMode(bool* wantFan, bool* wantFeeder) {
  if ((isTimerEnabled(FEEDER_RUN_TIMER) == false) &&
      (isTimerEnabled(FEEDER_REPEAT_TIMER) == false)) {
     
     startTimer(FEEDER_RUN_TIMER);
  }

  *wantFan = true;
  *wantFeeder = isTimerEnabled(FEEDER_RUN_TIMER);
}

void updateInKeepAliveMode(bool* wantFan, bool* wantFeeder) {
  //Keep fire alive
    //check if keep alive need to be activated?
    if ((isTimerEnabled(KEEP_ALIVE_REPEAT_TIMER) == false) &&
        (isTimerEnabled(KEEP_FIRE_ALIVE_FAN_TIMER) == false) &&
        (isTimerEnabled(KEEP_FIRE_ALIVE_FEEDER_TIMER) == false) ){
          
       //Keep alive need to be activated
       startTimer(KEEP_ALIVE_REPEAT_TIMER);
    }

    //control fan & feeder
    *wantFan = isTimerEnabled(KEEP_FIRE_ALIVE_FAN_TIMER);
    *wantFeeder = isTimerEnabled(KEEP_FIRE_ALIVE_FEEDER_TIMER);
}

void updateLogic() {
  bool wantFan = false;
  bool wantFeeder = false;

  //apply any logic?
  if (startedMode) {
    if (workModeRequested) {
      updateInWorkMode(&wantFan, &wantFeeder);
      //disable keep fire alive timers
      stopAndResetTimer(KEEP_ALIVE_REPEAT_TIMER);
      stopAndResetTimer(KEEP_FIRE_ALIVE_FAN_TIMER);
      stopAndResetTimer(KEEP_FIRE_ALIVE_FEEDER_TIMER);
      
    } else {
      updateInKeepAliveMode(&wantFan, &wantFeeder);
      //disable normal work mode timers
      stopAndResetTimer(FEEDER_RUN_TIMER);
      stopAndResetTimer(FEEDER_REPEAT_TIMER);
    }
  } else {
    //no logic is applied, stop all timers
    stopAndResetTimer(FEEDER_RUN_TIMER);
    stopAndResetTimer(FEEDER_REPEAT_TIMER);
    stopAndResetTimer(KEEP_ALIVE_REPEAT_TIMER);
    stopAndResetTimer(KEEP_FIRE_ALIVE_FAN_TIMER);
    stopAndResetTimer(KEEP_FIRE_ALIVE_FEEDER_TIMER);
  }

  //user requested feeder manualy?
  wantFeeder |= feederRequested;

  //check if feeder is to hot, if yes move coal ahead!
  if (lastFeederTemp > workConfig.feederTempCritical) {
    wantFeeder = true;
    wantFan = false;
  }

  //control feeder & fan
  if (wantFeeder) {
   digitalWrite(FEEDER_PIN, HIGH);
  } else {
    digitalWrite(FEEDER_PIN, LOW);
  }
  fanEnabled = wantFan;
}

void onFeederRunTimerEnd() {
  //end of feeding cycle, start repeat timer
  startTimer(FEEDER_REPEAT_TIMER);
}

void onFeederRepeatTimerEnd() {
  //end of repeat cycle, start feeder timer
  startTimer(FEEDER_RUN_TIMER);
}

void onKeepAliveRepeatTimerEnd() {
  startTimer(KEEP_FIRE_ALIVE_FAN_TIMER);
  startTimer(KEEP_FIRE_ALIVE_FEEDER_TIMER);
}

void measureFeederTempTimerEnd() {
  sensors.requestTemperatures();
  lastFeederTemp = (int)sensors.getTempCByIndex(0);
}

