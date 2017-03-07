#ifndef __COMMUNICATION__
#define __COMMUNICATION__

#include <inttypes.h>
#include <stdbool.h>

extern "C" {
typedef enum __attribute__ ((__packed__)) {

  
  cmdSetMinCWTemperature, cmdGetMinCWTemperature,
  cmdSetMaxCWTemperature, cmdGetMaxCWTemperature,
  cmdSetMinCOTemperature, cmdGetMinCOTemperature,
  cmdSetMaxCOTemperature, cmdGetMaxCOTemperature,

  cmdSetHeaterPumpCyclicRunDelay, cmdGetHeaterPumpCyclicRunDelay,
  cmdSetHeaterPumpCyclicRunDuration, cmdGetHeaterPumpCyclicRunDuration,

  cmdSetStartedMode,
  cmdSetWorkModeRequested,
  cmdSetFeederRequested,
  cmdGetLastFeederTemp,

  cmdSaveConfig, cmdResetToDefaultConfig, cmdLoadConfig
} CommandId;

typedef struct __attribute__ ((packed)) {
  uint8_t   typeAndUnit;
  uint8_t   minValue;
  uint8_t   maxValue;
  char*     name;
} ParamInfo;

void initCommunication();
}

#endif //__COMMUNICATION__
