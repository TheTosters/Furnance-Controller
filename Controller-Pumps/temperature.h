#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include <stdbool.h>

//Water in stove (C.O)
#define TEMP_CO 0

//Water for in home usage (C.W)
#define TEMP_CW 1

extern "C" {

extern float temperature[2];
extern bool sensorsError;

void beginTemperatures();
void updateTemperatures();

}
#endif
