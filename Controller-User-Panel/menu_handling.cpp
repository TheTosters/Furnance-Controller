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
#ifdef DEBUG_MENUS
  Serial.println("Menu:mainMenuWorkModeStartStop"); 
#endif
  if (workMode == WorkMode_STOP) {
    workMode = WorkMode_STANDBY;
  } else {
    workMode = WorkMode_STOP;
  }
  commLink->sendWorkMode(workMode);
}

static void clearButtonsCallbacks() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:clearButtonsCallbacks"); 
#endif
  Button* buttons[] = {buttonLeft, buttonRight, buttonExit, buttonEnter};
  for(uint8_t t = 0; t < 4; t++) {
    buttons[t]->setOnLongPressCallback(NULL);
    buttons[t]->setOnShortPressCallback(NULL);
  }
}

static void prevMenuItem() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:prevMenuItem"); 
#endif
  currentMenu->prevMenuItem();
}

static void nextMenuItem() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:nextMenuItem"); 
#endif
  currentMenu->nextMenuItem();
}

static void openMainMenu() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:openMainMenu"); 
#endif
  mainMenu->activate();
}

static void openFeederMenu() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:openFeederMenu"); 
#endif
  feederMenu->activate();  
}

static void openPumpsMenu() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:openPumpsMenu"); 
  Serial.println((int)pumpsMenu, HEX); 
#endif
  pumpsMenu->activate();
}

static void onSendChangedParam() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:onSendChangedParam");
#endif
  currentMenu->saveParamValue();
  isParamEditing = false;
}

static void editParamDec() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:editParamDec"); 
#endif
  currentMenu->decParam();
}

static void editParamInc() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:editParamInc"); 
#endif
  currentMenu->incParam();
}

static void onCancelParamEdit() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:onCancelParamEdit"); 
#endif
  isParamEditing = false;
  currentMenu->restoreParamValue();
  onRemoteMenuActivate();
}

static void startEditParam() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:startEditParam"); 
#endif
  buttonLeft->setOnShortPressCallback(editParamDec);
  buttonRight->setOnShortPressCallback(editParamInc);
  buttonExit->setOnShortPressCallback(onCancelParamEdit);
  buttonEnter->setOnShortPressCallback(onSendChangedParam);
  isParamEditing = true;
}

static void onRemoteMenuActivate() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:onRemoteMenuActivate"); 
#endif

  //no long presses, bind possibility to exit menu
  // Left, Right - menu navigation
  // Ok - select param to edit
  // Exit - move to main menu
  isParamEditing = false;
  clearButtonsCallbacks();
  buttonLeft->setOnShortPressCallback(prevMenuItem);
  buttonRight->setOnShortPressCallback(nextMenuItem);
  buttonExit->setOnShortPressCallback(openMainMenu);
  buttonEnter->setOnShortPressCallback(startEditParam);
}

static void onMainMenuActivate() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:onMainMenuActivate"); 
#endif
  isParamEditing = false;
  clearButtonsCallbacks();
  buttonLeft->setOnShortPressCallback(prevMenuItem);
  buttonRight->setOnShortPressCallback(nextMenuItem);

  buttonExit->setOnShortPressCallback(editParamDec);
  buttonEnter->setOnShortPressCallback(editParamInc);
}

void defineMenus() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:defineMenus"); 
#endif
  mainMenu = new Menu(0, mainMenuParams, 3, onMainMenuActivate);
  //feederMenu = commLink.getFeederMenu(onRemoteMenuActivate);
  pumpsMenu = commLink.getPumpsMenu(onRemoteMenuActivate);
  mainMenu->activate();
}

