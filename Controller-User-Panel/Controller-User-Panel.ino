/*
 * Connection info:
 * LCD AT PINS: D7, D6, D5, D4, D3, D2
 * BUTTONS AT PINS: D9, D10, D11, D12
 */

//DEBUGGING CONTROL
//#define DEBUG_KEYS 1
//#define DEBUG_FREE_MEM 1
//to debug menus go to menus.h and uncomment DEBUG_MENUS
//to debug communication for to communication.h and uncomment DEBUG_COMMUNICATION
//to debug Params set DEBUG_PARAMS in Params.h

#include "buttons.h"
#include "params.h"
#include "menus.h"
#include "menu_handling.h"
#include "communication.h"

#define BUTTON_LEFT_PIN   10    //D10
#define BUTTON_RIGHT_PIN  11    //D12
#define BUTTON_EXIT_PIN   9    //D11
#define BUTTON_ENTER_PIN  12     //D9

#define DEFAULT_LONGPRESS_LEN    15  // Min nr of loops for a long press

#define BUTTON_LEFT   1
#define BUTTON_RIGHT  2
#define BUTTON_ENTER  3
#define BUTTON_EXIT   4

void setup() {
  lcd.begin(16,2);
  Serial.begin(57600);
  Serial.println("---- BOOT ---");

  buttonLeft = new Button(BUTTON_LEFT, BUTTON_LEFT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonRight = new Button(BUTTON_RIGHT, BUTTON_RIGHT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonEnter = new Button(BUTTON_ENTER, BUTTON_EXIT_PIN, DEFAULT_LONGPRESS_LEN);
  buttonExit = new Button(BUTTON_EXIT, BUTTON_ENTER_PIN, DEFAULT_LONGPRESS_LEN);
  defineMenus();
}

#ifdef DEBUG_FREE_MEM
uint8_t * heapptr, * stackptr;
void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
  Serial.print("Free mem:");
  Serial.println((int)(stackptr-heapptr), DEC);
}
#endif

void loop() {
#ifdef DEBUG_FREE_MEM
  check_mem();
#endif
//  if (currentMenu == NULL) {
//    //this will communicate with other MCU and build valid menus for them
//    defineMenus();
//  }

  buttonLeft->update();
  buttonRight->update();
  buttonEnter->update();
  buttonExit->update();
#ifdef DEBUG_KEYS
  if (buttonLeft->isPressed()) {
    Serial.print("Left pressed\n");
  }
  if (buttonRight->isPressed()) {
    Serial.print("Right pressed\n");
  }
  if (buttonEnter->isPressed()) {
    Serial.print("Enter pressed\n");
  }
  if (buttonExit->isPressed()) {
    Serial.print("Exit pressed\n");
  }
#endif

  commLink.getGeneralUpdate();
  
  currentMenu->render();
}

