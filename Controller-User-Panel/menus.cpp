#include "menus.h"
#include <LiquidCrystal.h>
#include "params.h"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Menu* currentMenu = NULL;
static Param* currentParam = NULL;

Menu::Menu(uint8_t deviceId, Param** menuItems, uint8_t count, ExecMenuCallback onAct) 
: remoteDeviceId(deviceId), params(menuItems), paramsCount(count), currentIndex(0), onActivate(onAct) {
  
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

void Menu::nextMenuItem() {
  currentMenu->currentIndex++;
  if (currentMenu->currentIndex >= currentMenu->paramsCount) {
    currentMenu->currentIndex = 0;
  }
  currentParam = currentMenu->params[currentMenu->currentIndex];
}

void Menu::prevMenuItem() {
  currentMenu->currentIndex--;
  if (currentMenu->currentIndex < 0 ) {
    currentMenu->currentIndex = currentMenu->paramsCount - 1;
  }
  currentParam = currentMenu->params[currentMenu->currentIndex];
}

void Menu::incParam() {
  currentParam->inc();
}

void Menu::decParam() {
  currentParam->dec();
}

void Menu::getCurrentParamInfo(uint8_t* deviceId, uint8_t* paramIndex, uint8_t* paramValue) {
  ValuedParam* tmp = static_cast<ValuedParam*>(currentParam);
  *deviceId = remoteDeviceId;
  *paramIndex = tmp->getRemoteIndex();
  *paramValue = tmp->getValue();
}

