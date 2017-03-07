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

static void refreshCurrentParamValue() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:refreshCurrentParamValue"); 
#endif
  uint8_t deviceId;
  uint8_t paramIndex;
  uint8_t paramValue;
  currentMenu->getCurrentParamInfo(&deviceId, &paramIndex, &paramValue);
#ifdef DEBUG_MENUS
  Serial.print("  deviceId=");
  Serial.print(deviceId); 
  Serial.print(", paramIndex="); 
  Serial.print(paramIndex); 
  Serial.print(", paramValue="); 
  Serial.println(paramValue); 
#endif
/* todo: debug test
  if (deviceId != 0) {
    paramValue = commLink.requestParamValue(deviceId, paramIndex);
    currentMenu->setCurrentParamValue(paramValue);
  }
  */
}

static void prevMenuItem() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:prevMenuItem"); 
#endif
  currentMenu->prevMenuItem();
  refreshCurrentParamValue();
}

static void nextMenuItem() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:nextMenuItem"); 
#endif
  currentMenu->nextMenuItem();
  refreshCurrentParamValue();
}

static void openMainMenu() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:openMainMenu"); 
#endif
  currentMenu = mainMenu;
  currentMenu->activate();
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
  uint8_t devId, index, value;
  currentMenu->getCurrentParamInfo(&devId, &index, &value);
  commLink.sendParamChange(devId, index, value); 
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

static void startEditParam() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:startEditParam"); 
#endif
  buttonLeft->setOnShortPressCallback(editParamDec);
  buttonRight->setOnShortPressCallback(editParamInc);
  buttonExit->setOnShortPressCallback(onRemoteMenuActivate);
  buttonEnter->setOnShortPressCallback(onSendChangedParam);
}

static void onRemoteMenuActivate() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:onRemoteMenuActivate"); 
#endif

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
#ifdef DEBUG_MENUS
  Serial.println("Menu:onMainMenuActivate"); 
#endif
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
  currentMenu = mainMenu;
}
