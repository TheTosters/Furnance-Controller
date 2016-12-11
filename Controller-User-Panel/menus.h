#ifndef __MENUS_H__
#define __MENUS_H__

#include <Arduino.h>
#include <LiquidCrystal.h>

class Param;
class Button;
class Menu;

extern Button* buttonLeft;
extern Button* buttonRight;
extern Button* buttonExit;
extern Button* buttonEnter;
extern LiquidCrystal lcd;
extern Menu* currentMenu;

typedef void (*ExecMenuCallback)();

class Menu {
  public:
    Menu(Param** menuItems, uint8_t count, ExecMenuCallback onActivate);
    
    void      render();
    void      activate();
  public:
    Param**   params;
    uint8_t   paramsCount;
    int8_t    currentIndex;
    ExecMenuCallback onActivate;
};

void defineMenus();
#endif
