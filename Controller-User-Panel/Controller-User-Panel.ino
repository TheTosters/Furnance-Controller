#include "buttons.h"
#include "params.h"
#include "menus.h"
#include "menu_handling.h"
#include "communication.h"

#define BUTTON_LEFT_PIN   2
#define BUTTON_RIGHT_PIN  3
#define BUTTON_EXIT_PIN   4
#define BUTTON_ENTER_PIN  5

#define DEFAULT_LONGPRESS_LEN    25  // Min nr of loops for a long press

#define BUTTON_LEFT   1
#define BUTTON_RIGHT  2
#define BUTTON_ENTER  3
#define BUTTON_EXIT   4

void setup() {
  lcd.begin(26,2);
  buttonLeft = new Button(BUTTON_LEFT, BUTTON_LEFT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonRight = new Button(BUTTON_RIGHT, BUTTON_RIGHT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonEnter = new Button(BUTTON_ENTER, BUTTON_EXIT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonExit = new Button(BUTTON_EXIT, BUTTON_ENTER_PIN, DEFAULT_LONGPRESS_LEN);
}

void loop() {
  if (currentMenu == NULL) {
    //this will communicate with other MCU and build valid menus for them
    defineMenus();
    currentMenu->activate();    
  }
  
  buttonLeft->update();
  buttonRight->update();
  buttonEnter->update();
  buttonExit->update();
  commLink.getGeneralUpdate();
  currentMenu->render();
}
