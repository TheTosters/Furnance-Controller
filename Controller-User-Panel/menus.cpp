#include "menus.h"
#include <LiquidCrystal.h>
#include "params.h"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Menu* currentMenu = NULL;
static Param* currentParam = NULL;

static void nextMenuItem(uint8_t id) {
  currentMenu->currentIndex++;
  if (currentMenu->currentIndex >= currentMenu->paramsCount) {
    currentMenu->currentIndex = 0;
  }
  currentParam = currentMenu->params[currentMenu->currentIndex];
}

static void prevMenuItem(uint8_t id) {
  currentMenu->currentIndex--;
  if (currentMenu->currentIndex < 0 ) {
    currentMenu->currentIndex = currentMenu->paramsCount - 1;
  }
  currentParam = currentMenu->params[currentMenu->currentIndex];
}

static void incParam(uint8_t id) {
  currentParam->inc();
}

static void decParam(uint8_t id) {
  currentParam->dec();
}

Menu::Menu(Param** menuItems, uint8_t count, ExecMenuCallback onAct) 
: params(menuItems), paramsCount(count), currentIndex(0), onActivate(onAct) {
  
}
    
void Menu::render() {
  char line1[17], line2[17];
  params[currentIndex]->getLCDLines(line1, line2);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void Menu::activate() {
  currentMenu = this;
  if (onActivate != NULL) {
    onActivate();
  }
}

static void mainMenuWorkModeStartStop() {
  if (workMode == WorkMode_STOP) {
    workMode = WorkMode_STANDBY;
  } else {
    workMode = WorkMode_STOP
  }
}

static Menu* mainMenu;
static Param* mainMenuParams[] = {
  new SpecialParam(&coTemp, &cwTemp, &workMode, mainMenuWorkModeStartStop, NULL)
};

void onMainMenuActivate() {
  
}

void defineMenus() {
  mainMenu = new Menu(mainMenuParams, sizeof(mainMenuParams), onMainMenuActivate);
  currentMenu = mainMenu;
}

