#ifndef __LOGIC__
#define __LOGIC__

#include <inttypes.h>
#include <stdbool.h>

// Output to Opto Triac pin
#define AC_LOAD 3

#define FEEDER_PIN 8
#define TEMP_SENSOR_ONE_WIRE_PIN 9

//in work mode counts time of feeder working.
#define FEEDER_RUN_TIMER  0

//in work mode, should be executed after end of FEEDER_RUN_TIMER.
#define FEEDER_REPEAT_TIMER  1

//If not in work mode, count time to run keep fire alive procedure
#define KEEP_ALIVE_REPEAT_TIMER     2

//in Keep fire alive mode, counts time how long fan should be run
#define KEEP_FIRE_ALIVE_FAN_TIMER     3

//in Keep fire alive mode, counts time how long feeder should be run
#define KEEP_FIRE_ALIVE_FEEDER_TIMER     4

//used for cyclic temperature measurements
#define FEEDER_TEMP_CHECK_TIMER     5

extern "C" {

typedef struct {
  //all timers in ms
  uint32_t feederRunTime;
  uint32_t feederRepeatTime;
  uint32_t keepAliveRepeatTime;
  uint32_t keepAliveFanTime;
  uint32_t keepAliveFeederTime;
  int feederTempCritical;
  int fanPower;
} WorkConfig;

extern WorkConfig workConfig;
extern bool fanEnabled;
extern bool startedMode;
extern bool workModeRequested;

void initLogic();
void updateLogic();

void resetConfigToDefault();
void saveConfig();
void loadConfig();

//timers handlers
void onFeederRunTimerEnd();
void onFeederRepeatTimerEnd();
void onKeepAliveRepeatTimerEnd();
void measureFeederTempTimerEnd();
}
#endif
