#ifndef __MENUS_H__
#define __MENUS_H__

#include <Arduino.h>
#include <LiquidCrystal.h>

#define DEBUG_MENUS 1

class Param;
class Button;
class Menu;

extern Button* buttonLeft;
extern Button* buttonRight;
extern Button* buttonExit;
extern Button* buttonEnter;
extern LiquidCrystal lcd;
extern Menu* currentMenu;

typedef void (*ExecMenuCallback)(void);

class Menu {
  public:
    Menu(uint8_t remoteDeviceId, Param** menuItems, uint8_t count, ExecMenuCallback onActivate);
    ~Menu();
    void      render();
    void      activate();
    void      nextMenuItem();
    void      prevMenuItem();
    void      incParam();
    void      decParam();
    void      getCurrentParamInfo(uint8_t* deviceId, uint8_t* paramIndex, uint8_t* paramValue);
    void      setCurrentParamValue(uint8_t value);
    void      resetCurrentParam();
  public:
    Param**   params;
    uint8_t   paramsCount;
    int8_t    currentIndex;
    uint8_t   remoteDeviceId;
    ExecMenuCallback onActivate;
};

#endif
