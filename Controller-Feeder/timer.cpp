#include <Arduino.h>
#include "timer.h"

Timer globalTimer;

void updateTimers() {
  unsigned long now = millis();
  int delta = (int)(now - globalTimer.lastMilis);
  globalTimer.lastMilis = now;
  
  for(int t = 0; t < MAX_EVENTS; t++) {
    SubTimer* tmp = &globalTimer.timers[t];

    if (bitRead(tmp->flags, FLAG_IS_ACTIVE) == 0) {
      continue;
    }
    tmp->timeToConsume -= delta;
    if (tmp->timeToConsume <= 0) {
      if (bitRead(tmp->flags, FLAG_AUTO_REPEAT) != 0) {
        startTimer(t);
        
      } else {
        stopAndResetTimer(t);
      }
    }
  }
}

void startTimer(uint8_t timerId) {
  SubTimer* tmp = &globalTimer.timers[timerId];
  tmp->timeToConsume = tmp->repeatTime;
  bitSet(tmp->flags, FLAG_IS_ACTIVE);
  if (tmp->onTimerStart != NULL) {
    tmp->onTimerStart();
  }
}

void stopAndResetTimer(uint8_t timerId) {
  if (isTimerEnabled(timerId) == false) {
    return;
  }
  
  SubTimer* tmp = &globalTimer.timers[timerId];
  tmp->timeToConsume = 0;
  bitClear(tmp->flags, FLAG_IS_ACTIVE);
  if (tmp->onTimerEnd != NULL) {
    tmp->onTimerEnd();
  }
}

void defineSubTimer(uint8_t timerId, int periodTime, bool autorepeats, TimerCallback startCb, TimerCallback endCb) {
  SubTimer* tmp = &globalTimer.timers[timerId];
  tmp->repeatTime = periodTime;
  tmp->timeToConsume = 0;
  tmp->flags = 0;
  if (autorepeats){
    bitSet(tmp->flags, FLAG_AUTO_REPEAT);
  }
  tmp->onTimerStart = startCb;
  tmp->onTimerEnd = endCb;
}

void setTimerEnabled(uint8_t timerId, bool isEnabled) {
  if (isEnabled) {
    bitSet(globalTimer.timers[timerId].flags, FLAG_IS_ACTIVE);

  } else {
    bitClear(globalTimer.timers[timerId].flags, FLAG_IS_ACTIVE);
  }
}

bool isTimerEnabled(uint8_t timerId) {
  return bitRead(globalTimer.timers[timerId].flags, FLAG_IS_ACTIVE) != 0;
}

