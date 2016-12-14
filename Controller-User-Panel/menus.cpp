#include "menus.h"
#include <LiquidCrystal.h>
#include "params.h"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Menu* currentMenu = NULL;
static Param* currentParam = NULL;
static char oldLine1[17], oldLine2[17];

Menu::Menu(uint8_t deviceId, Param** menuItems, uint8_t count, ExecMenuCallback onAct) 
: remoteDeviceId(deviceId), params(menuItems), paramsCount(count), currentIndex(0), onActivate(onAct) {
  
}
    
void Menu::render() {
  char line1[17], line2[17];
  params[currentIndex]->getLCDLines(line1, line2);
  
  if ((strncmp(line1, oldLine1, 16) == 0) &&
      (strncmp(line2, oldLine2, 16) == 0)) {
     return;
  }

  strncpy(oldLine1, line1, 16);
  strncpy(oldLine2, line2, 16);
  
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

void Menu::setCurrentParamValue(uint8_t value) {
  if (currentParam->isRemotelyRefreshable()) {
    ValuedParam* tmp = static_cast<ValuedParam*>(currentParam);
    tmp->setValue(value);
  }
}

void Menu::getCurrentParamInfo(uint8_t* deviceId, uint8_t* paramIndex, uint8_t* paramValue) {
  if (currentParam->isRemotelyRefreshable()) {
    ValuedParam* tmp = static_cast<ValuedParam*>(currentParam);
    *deviceId = remoteDeviceId;
    *paramIndex = tmp->getRemoteIndex();
    *paramValue = tmp->getValue();

  } else {
    *deviceId = 0;
    *paramIndex = 0;
    *paramValue = 0;
  }
}

