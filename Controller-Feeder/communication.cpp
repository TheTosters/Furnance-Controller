#include <Wire.h>
#include "communication.h"
#include "logic.h"
#include "communication_consts.h"

//master want info about how many params can be configured in this MCU
#define PARAM_GET_MENU_ITEMS_COUNT 1

//master requested information about max/min/type of param, this is start offset, so if param info is requested
//then in requestedParam will be value (PARAM_INFO_OFFSET + param index)
#define PARAM_INFO_OFFSET 30

//master requested information about textual name of param, this is start offset, so if param info is requested
//then in requestedParam will be value (PARAM_INFO_OFFSET + param index)
#define PARAM_NAME_OFFSET 60

//master requested value of some param, this is start offset, so if param value is requested
//then in requestedParam will be value (PARAM_VALUE_OFFSET + param index)
#define PARAM_VALUE_OFFSET 90

typedef enum {
  WorkMode_STOP,
  WorkMode_RUNNING,
  WorkMode_STANDBY  
} WorkMode;

//if master i2c want some data, this variable is used to determine what we want!
static uint8_t requestedParam;

static ParamInfo paramsInfo[] = {
  {.type = 0, .minValue=0, .maxValue=0, .name=""}
};

//forward declarations
void onI2cReceiveEvent(int bytesCount);
void onI2cRequest();

bool requestInRange(uint8_t startRange, uint8_t endRange) {
  return (requestedParam >= startRange) && (requestedParam <= endRange);
}

void initCommunication() {
  Wire.begin(I2C_DEVICE_FEEDER);
  Wire.onRequest(onI2cRequest);
  Wire.onReceive(onI2cReceiveEvent);
}

void onI2cRequest() {
  if (requestedParam == PARAM_GET_MENU_ITEMS_COUNT) {
    Wire.write(14);
    
  } else if (requestInRange(PARAM_INFO_OFFSET, PARAM_INFO_OFFSET + 29)) {
    int index = requestedParam - PARAM_INFO_OFFSET;
    Wire.write(paramsInfo[index].type);
    Wire.write(paramsInfo[index].minValue);
    Wire.write(paramsInfo[index].maxValue);
    Wire.write(strlen(paramsInfo[index].name));
    
  } else if (requestInRange(PARAM_NAME_OFFSET, PARAM_NAME_OFFSET + 29)) {
    int index = requestedParam - PARAM_NAME_OFFSET;
    char* namePtr = paramsInfo[index].name;
    while(*namePtr != NULL) {
      Wire.write(*namePtr);
      namePtr++;
    }
    
  } else if (requestInRange(PARAM_VALUE_OFFSET, PARAM_VALUE_OFFSET + 29)) {
    //TODO: implement
    switch(requestedParam - PARAM_VALUE_OFFSET) {
      case 0:
      break;
    }
  }
}

/*
void handleReceivedCommand(Command* command) {
  switch(command->cmd) {
    case cmdSetFeederRunTime:
      workConfig.feederRunTime = command->argument;
      break;
      
    case cmdGetFeederRunTime:
      sendCmd(command->cmd, workConfig.feederRunTime);
      break;
        
    case cmdSetFeederRepeatTime:
      workConfig.feederRepeatTime = command->argument;
      break;
      
    case cmdGetFeederRepeatTime:
      sendCmd(command->cmd, workConfig.feederRepeatTime);
      break; 
      
    case cmdSetKeepAliveRepeatTime:
      workConfig.keepAliveRepeatTime = command->argument;
      break;
      
    case cmdGetKeepAliveRepeatTime:
      sendCmd(command->cmd, workConfig.keepAliveRepeatTime);
      break; 
      
    case cmdSetKeepAliveFanTime:
      workConfig.keepAliveFanTime = command->argument;
      break;
      
    case cmdGetKeepAliveFanTime:
      sendCmd(command->cmd, workConfig.keepAliveFanTime);
      break;
      
    case cmdSetKeepAliveFeederTime: 
      workConfig.keepAliveFeederTime = command->argument;
      break;
      
    case cmdGetKeepAliveFeederTime:
      sendCmd(command->cmd, workConfig.keepAliveFeederTime);
      break;
      
    case cmdSetFeederTempCritical:
      workConfig.feederTempCritical = command->argument;
      break;
      
    case cmdGetFeederTempCritical:
      sendCmd(command->cmd, workConfig.feederTempCritical);
      break;
      
    case cmdSetFanPower:
      workConfig.fanPower = command->argument;
      workConfig.fanPower = workConfig.fanPower > 128 ? 128 : workConfig.fanPower;
      workConfig.fanPower = workConfig.fanPower < 0 ? 0 : workConfig.fanPower;
      break;
      
    case cmdGetFanPower:
      sendCmd(command->cmd, workConfig.fanPower);
      break;
      
    case cmdSetStartedMode:
      startedMode = cmd->argument;
      break;
      
    case cmdSetWorkModeRequested:
      workModeRequested = cmd->argument;
      break;
      
    case cmdSetFeederRequested:
    case cmdGetLastFeederTemp:
    case cmdSaveConfig:
    case cmdResetToDefaultConfig:
    case cmdLoadConfig:
      break;
  }
}
  */
void changeParamValue(uint8_t paramIndex, uint8_t value) {
  //todo: implement
}

void onI2cReceiveEvent(int bytesCount) {

  uint8_t cmd = Wire.read();
  uint8_t cmdValue = (bytesCount == 2) ? Wire.read() : 0;

  switch(cmd) {
    case I2C_CMD_GENERAL_SET_WORKMODE:
      startedMode = cmdValue != WorkMode_STOP;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM:
      requestedParam = PARAM_VALUE_OFFSET + cmdValue;
      break;
    
    case I2C_CMD_GENERAL_SET_PARAM:
      changeParamValue(cmdValue, Wire.read());
      break;
    
    case I2C_CMD_GENERAL_GET_PARAMS_COUNT:
      requestedParam = PARAM_GET_MENU_ITEMS_COUNT;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_DESCRIPTION:
      requestedParam = PARAM_INFO_OFFSET + cmdValue;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_NAME:
      requestedParam = PARAM_NAME_OFFSET + cmdValue;
      break;
    
  }
}
