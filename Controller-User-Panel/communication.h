
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
    void sendParamChange(uint8_t deviceId, uint8_t index, uint8_t newValue);
    uint8_t requestParamValue(uint8_t deviceId, uint8_t index);

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
