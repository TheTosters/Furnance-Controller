#include <Wire.h>
#include "communication.h"
#include "logic.h"

//forward declarations
void onI2cReceiveEvent(int bytesCount);

void initCommunication() {
  Wire.begin(1);
//  Wire.onRequest(onI2cRequest);
  Wire.onReceive(onI2cReceiveEvent);
}

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

void onI2cReceiveEvent(int bytesCount) {
  Command command;
  uint8_t* cmdPtr = (uint8_t*)&command;
  int size = sizeof(command);
  
  if (size != bytesCount) {
    //something wrong... :(
    //TODO: Inform user about this
  }
  
  while ( (size > 0) && (Wire.available() > 1) ) { 
    *cmdPtr = Wire.read();
    cmdPtr ++;
    size --;
  }

  handleReceivedCommand(&command);
}
