
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <Arduino.h>

class Communication {
  public:
    Communication();

    //general purpouse commands
    void sendWantFeederCommand(bool wantFeeder);
    void sendLoadCoalCommand(bool doLoad);
    void sendWorkMode(int8_t mode);

    //feeder device commands
    void requestFeederParam(uint8_t index);
    void sendFeederParamChange(uint8_t index, uint8_t newValue);

    //pumps device commands
    void requestPumpsParam(uint8_t index);
    void sendPumpsParamChange(uint8_t index, uint8_t newValue);

    //used in main menu, to show CO/CW temperatures, work times, etc
    void getGeneralUpdate();
  private:
    
};

extern Communication commLink;

#endif
