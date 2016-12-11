
#include "communication.h"
#include "logic.h"
#define MAX_EVENTS  6
#include "timer.h"

void setup() {
  initLogic();
  initCommunication();
  globalTimer.lastMilis = 0;

  defineSubTimer(FEEDER_RUN_TIMER, workConfig.feederRunTime, false, NULL, onFeederRunTimerEnd);
  defineSubTimer(FEEDER_REPEAT_TIMER, workConfig.feederRepeatTime, false, NULL, onFeederRepeatTimerEnd);
  
  defineSubTimer(KEEP_ALIVE_REPEAT_TIMER, workConfig.keepAliveRepeatTime, true, NULL, onKeepAliveRepeatTimerEnd);
  defineSubTimer(KEEP_FIRE_ALIVE_FAN_TIMER, workConfig.keepAliveFanTime, false, NULL, NULL);
  defineSubTimer(KEEP_FIRE_ALIVE_FEEDER_TIMER, workConfig.keepAliveFeederTime, false, NULL, NULL);

  defineSubTimer(FEEDER_TEMP_CHECK_TIMER, 10 * 1000, true, NULL, measureFeederTempTimerEnd);
  pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
  attachInterrupt(0, zero_crosss_int, RISING);  
}

// function to be fired at the zero crossing to dim the light
void zero_crosss_int()  
{
  if (fanEnabled == false) {
    digitalWrite(AC_LOAD, HIGH);
    return;
  }
  
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms  
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) For 60Hz => 8.33ms

  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65
  int dimtime = (75 * workConfig.fanPower);   // For 60Hz =>65     
  delayMicroseconds(dimtime);                 // Off cycle
  digitalWrite(AC_LOAD, LOW);                 // triac firing
  delayMicroseconds(10);                      // triac On propogation delay (for 60Hz use 8.33)
  digitalWrite(AC_LOAD, HIGH);                // triac Off
}

void loop() {
  updateTimers();
  updateLogic();
  delay(200);
}
