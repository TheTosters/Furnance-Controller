#include "menus.h"
#include <LiquidCrystal.h>
#include "params.h"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  //PINS: D7, D6, D5, D4, D3, D2

Menu* currentMenu = NULL;
static Param* currentParam = NULL;
//static char oldLine1[17]={0}, oldLine2[17]={0};
static uint8_t oldLine1Hash = 0, oldLine2Hash = 0;

int8_t crc8(const uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;

    while (len--) {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

Menu::Menu(uint8_t deviceId, Param** menuItems, uint8_t count, ExecMenuCallback onAct) 
: remoteDeviceId(deviceId), params(menuItems), paramsCount(count), currentIndex(0), onActivate(onAct) {
  
}

Menu::~Menu() {
  Serial.println("DESTROY MENU");
}

void Menu::render() {
  char line1[17]={0}, line2[17]={0};
  params[currentIndex]->getLCDLines(line1, line2);
  uint8_t tmpHash = crc8(line1, 16);
  if (tmpHash == oldLine1Hash) {
    tmpHash = crc8(line2, 16);
    if (tmpHash == oldLine2Hash) {
      return;
    } else {
      oldLine2Hash = tmpHash;
    }
  } else {
    oldLine1Hash = tmpHash;
  }
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void Menu::activate() {
#ifdef DEBUG_MENUS
  Serial.println("Menu:Activate");
#endif
  currentMenu = this;
  resetCurrentParam();
  if (onActivate != NULL) {
    onActivate();
  }
#ifdef DEBUG_MENUS
  Serial.print("Menu:Activate current param index=");
  Serial.print(currentIndex);
  Serial.print(" currParamAddr:");
  Serial.println((int)currentParam, HEX);
#endif
}

void Menu::resetCurrentParam() {
  currentParam = params[currentIndex];  
}

void Menu::nextMenuItem() {
  currentIndex++;
  if (currentIndex >= paramsCount) {
    currentIndex = 0;
  }
  resetCurrentParam();
#ifdef DEBUG_MENUS
  Serial.print("NextMenuItem: newIndex=");
  Serial.print(currentIndex);
  Serial.print(", currentMenu=");
  Serial.print((long int)currentMenu, HEX);
  Serial.print(", param=");
  Serial.println((long int)currentParam, HEX);
#endif
}

void Menu::prevMenuItem() {
  currentIndex--;
  if (currentIndex < 0 ) {
    currentIndex = paramsCount - 1;
  }
  resetCurrentParam();
#ifdef DEBUG_MENUS
  Serial.print("PrevMenuItem: newIndex=");
  Serial.print(currentIndex);
  Serial.print(", currentMenu=");
  Serial.print((long int)currentMenu, HEX);
  Serial.print(", param=");
  Serial.println((long int)currentParam, HEX);
#endif
}

void Menu::incParam() {
#ifdef DEBUG_MENUS
  if (currentParam == NULL) {
    Serial.println("Menu::incParam currentParam == NULL");
    return;
  }
#endif
  currentParam->inc();
}

void Menu::decParam() {
#ifdef DEBUG_MENUS
  if (currentParam == NULL) {
    Serial.println("Menu::decParam currentParam == NULL");
    return;
  }
#endif
  currentParam->dec();
}

void Menu::setCurrentParamValue(uint8_t value) {
#ifdef DEBUG_MENUS
  if (currentParam == NULL) {
    Serial.println("Menu::setCurrentParamValue currentParam == NULL");
    return;
  }
#endif
  
  if (currentParam->isRemotelyRefreshable()) {
    ValuedParam* tmp = static_cast<ValuedParam*>(currentParam);
    tmp->setValue(value);
  }
}

void Menu::getCurrentParamInfo(uint8_t* deviceId, uint8_t* paramIndex, uint8_t* paramValue) {
#ifdef DEBUG_MENUS
  if (currentParam == NULL) {
    Serial.println("Menu::getCurrentParamInfo currentParam == NULL");
    return;
  }
#endif
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

