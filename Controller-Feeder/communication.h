#ifndef __COMMUNICATION__
#define __COMMUNICATION__

#include <inttypes.h>
#include <stdbool.h>

extern "C" {
typedef enum __attribute__ ((__packed__)) {
  cmdSetFeederRunTime, cmdGetFeederRunTime,
  cmdSetFeederRepeatTime, cmdGetFeederRepeatTime,
  cmdSetKeepAliveRepeatTime, cmdGetKeepAliveRepeatTime,
  cmdSetKeepAliveFanTime, cmdGetKeepAliveFanTime,
  cmdSetKeepAliveFeederTime, cmdGetKeepAliveFeederTime,
  cmdSetFeederTempCritical, cmdGetFeederTempCritical,
  cmdSetFanPower, cmdGetFanPower,

  cmdSetStartedMode,
  cmdSetWorkModeRequested,
  cmdSetFeederRequested,
  cmdGetLastFeederTemp,

  cmdSaveConfig, cmdResetToDefaultConfig, cmdLoadConfig
} CommandId;

typedef struct __attribute__ ((packed)) {
  uint8_t   type;
  uint8_t   minValue;
  uint8_t   maxValue;
  char*     name;
} ParamInfo;

void initCommunication();
}

#endif //__COMMUNICATION__
