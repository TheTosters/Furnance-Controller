#ifndef __TIMER__
#define __TIMER__

#include <inttypes.h>
#include <stdbool.h>

#ifndef MAX_EVENTS
#define MAX_EVENTS 6
#endif 

//time calculations are done only if this is set
#define FLAG_IS_ACTIVE    1
//if set then after end events immediately starts
#define FLAG_AUTO_REPEAT  2

extern "C" {
typedef void (*TimerCallback)();

typedef struct {
   int repeatTime;      //the period of repeats
   int timeToConsume;   //how many ms until next triger
   uint8_t flags;
   TimerCallback onTimerStart;
   TimerCallback onTimerEnd;
} SubTimer;

typedef struct {
  SubTimer timers[MAX_EVENTS];
  unsigned long lastMilis;
} Timer;

extern Timer globalTimer;

void updateTimers();
void startTimer(uint8_t timerId);
void stopAndResetTimer(uint8_t timerId);
void defineSubTimer(uint8_t timerId, int periodTime, bool autorepeats, TimerCallback startCb, TimerCallback endCb);
void setTimerEnabled(uint8_t timerId, bool isEnabled);
bool isTimerEnabled(uint8_t timerId);

}
#endif //__TIMER__
