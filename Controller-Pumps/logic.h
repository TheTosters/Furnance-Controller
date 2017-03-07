#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <stdbool.h>
#include <stdint.h>

//Pump for inner circuit (stove -> heat exchanger)
//this is digital PIN number
#define PUMP_CO 3

//Pump for CO-CW circuit
//this is digital PIN number
#define PUMP_CW 4

//pump for (heat exchanger -> heaters)
//this is digital PIN number
#define PUMP_HEATERS 5

extern "C" {

typedef struct {
  int8_t minCWTemp;
  int8_t maxCWTemp;

  int8_t minCOTemp;
  int8_t maxCOTemp;

  int32_t    heaterPumpCyclicRunDelay;
  int32_t    heaterPumpCyclicRunDuration;
} LogicConfig;

extern LogicConfig logicConfig;

//if true then remote control want to run heaters in home
extern bool wantHeaters;

//if true, then feeder & fan should be run, we want fire!
extern bool wantFeederRun;

//probably not used here
extern bool startedMode;

void beginLogic();
void updateLogic();
void pumpRunning(uint8_t pompId, bool isOn);

}
#endif
