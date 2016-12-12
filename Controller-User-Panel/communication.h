
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <Arduino.h>
#include "menus.h"

typedef void (*ParamDescriptionCallback)(Param* param);

class Communication {
  public:
    Communication();

    //general purpouse commands
    void sendWantFeederCommand(bool wantFeeder);
    void sendLoadCoalCommand(bool doLoad);
    void sendWorkMode(int8_t mode);

    //feeder device commands
    uint8_t   requestFeederParam(uint8_t index);
    void      sendFeederParamChange(uint8_t index, uint8_t newValue);

    //pumps device commands
    uint8_t   requestPumpsParam(uint8_t index);
    void      sendPumpsParamChange(uint8_t index, uint8_t newValue);

    //used in main menu, to show CO/CW temperatures, work times, etc
    void getGeneralUpdate();

    //used to fetch info from other MCU
    Menu* getFeederMenu(ExecMenuCallback onActivate);
    Menu* getPumpsMenu(ExecMenuCallback onActivate);
  private:
    Menu* getMenu(int deviceId, ExecMenuCallback onActivate);
};

extern Communication commLink;

#endif
