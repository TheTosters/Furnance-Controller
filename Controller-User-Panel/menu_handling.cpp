#include "menu_handling.h"
#include "menus.h"
#include "params.h"
#include "communication.h"

Menu* mainMenu;
Menu* feederMenu;
Menu* pumpsMenu;

Button* buttonLeft;
Button* buttonRight;
Button* buttonExit;
Button* buttonEnter;
WorkMode workMode;
uint8_t coTemp;
uint8_t cwTemp;

static void mainMenuWorkModeStartStop();
static void onRemoteMenuActivate();

static Param* mainMenuParams[] = {
  new SpecialParam(&coTemp, &cwTemp, &workMode, mainMenuWorkModeStartStop, NULL)
};

static void mainMenuWorkModeStartStop() {
  if (workMode == WorkMode_STOP) {
    workMode = WorkMode_STANDBY;
  } else {
    workMode = WorkMode_STOP;
  }
}

static void clearButtonsCallbacks() {
  Button* buttons[] = {buttonLeft, buttonRight, buttonExit, buttonEnter};
  for(uint8_t t = 0; t < 4; t++) {
    buttons[t]->setOnLongPressCallback(NULL);
    buttons[t]->setOnShortPressCallback(NULL);
  }
}

static void onMainMenuActivate() {
  clearButtonsCallbacks();
  //TODO: write key binding!
}

static void prevMenuItem() {
  currentMenu->prevMenuItem();
}

static void nextMenuItem() {
  currentMenu->nextMenuItem();
}

static void openMainMenu() {
  currentMenu = mainMenu;
  currentMenu->activate();
}

static void onSendChangedParam() {
  //TODO: sending param to I2C device
}

static void editParamDec() {
  currentMenu->decParam();
}

static void editParamInc() {
  currentMenu->incParam();
}

static void startEditParam() {
  buttonLeft->setOnShortPressCallback(editParamDec);
  buttonRight->setOnShortPressCallback(editParamInc);
  buttonExit->setOnShortPressCallback(onRemoteMenuActivate);
  buttonEnter->setOnShortPressCallback(onSendChangedParam);
}

static void onRemoteMenuActivate() {
  //no long presses, bind possibility to exit menu
  // Left, Right - menu navigation
  // Ok - select param to edit
  // Exit - move to main menu
  clearButtonsCallbacks();
  buttonLeft->setOnShortPressCallback(prevMenuItem);
  buttonRight->setOnShortPressCallback(nextMenuItem);
  buttonExit->setOnShortPressCallback(openMainMenu);
  buttonEnter->setOnShortPressCallback(startEditParam);
}

void defineMenus() {
  mainMenu = new Menu(mainMenuParams, sizeof(mainMenuParams), onMainMenuActivate);
  feederMenu = commLink.getFeederMenu(onRemoteMenuActivate);
  pumpsMenu = commLink.getPumpsMenu(onRemoteMenuActivate);
  currentMenu = mainMenu;
}

