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
static void openFeederMenu();
static void openPumpsMenu();

static Param* mainMenuParams[] = {
  new SpecialParam(&coTemp, &cwTemp, &workMode, NULL, mainMenuWorkModeStartStop),
  new ExecParam("Menu podajnika", openFeederMenu, NULL),
  new ExecParam("Menu pomp", openPumpsMenu, NULL),
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

static void refreshCurrentParamValue() {
  uint8_t deviceId;
  uint8_t paramIndex;
  uint8_t paramValue;
  currentMenu->getCurrentParamInfo(&deviceId, &paramIndex, &paramValue);
  if (deviceId != NULL) {
    paramValue = commLink.requestParamValue(deviceId, paramIndex);
    currentMenu->setCurrentParamValue(paramValue);
  }
}

static void prevMenuItem() {
  currentMenu->prevMenuItem();
  refreshCurrentParamValue();
}

static void nextMenuItem() {
  currentMenu->nextMenuItem();
  refreshCurrentParamValue();
}

static void openMainMenu() {
  currentMenu = mainMenu;
  currentMenu->activate();
}

static void openFeederMenu() {
  currentMenu = feederMenu;
  currentMenu->activate();  
}

static void openPumpsMenu() {
  currentMenu = pumpsMenu;
  currentMenu->activate();
}

static void onSendChangedParam() {
  uint8_t devId, index, value;
  currentMenu->getCurrentParamInfo(&devId, &index, &value);
  commLink.sendParamChange(devId, index, value); 
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

static void onMainMenuActivate() {
  clearButtonsCallbacks();
  buttonLeft->setOnShortPressCallback(prevMenuItem);
  buttonRight->setOnShortPressCallback(nextMenuItem);

  buttonExit->setOnShortPressCallback(editParamDec);
  buttonEnter->setOnShortPressCallback(editParamInc);
}

void defineMenus() {
  mainMenu = new Menu(0, mainMenuParams, sizeof(mainMenuParams), onMainMenuActivate);
  feederMenu = commLink.getFeederMenu(onRemoteMenuActivate);
  pumpsMenu = commLink.getPumpsMenu(onRemoteMenuActivate);
  currentMenu = mainMenu;
}

