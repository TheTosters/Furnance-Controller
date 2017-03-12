#ifndef __MENU_HANDLING_H__
#define __MENU_HANDLING_H__

#include <Arduino.h>
#include "buttons.h"
#include "params.h"

extern Button* buttonLeft;
extern Button* buttonRight;
extern Button* buttonExit;
extern Button* buttonEnter;
extern WorkMode workMode;
extern uint8_t coTemp;
extern uint8_t cwTemp;

void defineMenus();

#endif

