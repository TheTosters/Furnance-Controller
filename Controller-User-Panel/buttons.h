#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <Arduino.h>

#ifndef BUTTON_CHECK_DELAY
// Delay per loop in ms
#define BUTTON_CHECK_DELAY  20
#endif

typedef enum { 
  EV_NONE=0, 
  EV_SHORTPRESS, 
  EV_LONGPRESS
} ButtonPressEvent;

typedef void (*ButtonCallback)();

class Button {
  public:
    Button(uint8_t id, uint8_t pin, uint8_t longpress_len);
    ButtonPressEvent update();

    bool isPressed();
    bool isLongPressed();
    void setOnLongPressCallback(ButtonCallback callback);
    void setOnShortPressCallback(ButtonCallback callback);
  protected:
    unsigned        wasPressed:1;      // previous state
    unsigned        nowPressed:1;      //current state
    ButtonCallback  onShortPress;
    ButtonCallback  onLongPress;
    unsigned long   lastMillis;
    uint8_t         delayTime;
    uint8_t         pressedCounter;   // press running duration
    uint8_t         id;
    const uint8_t   pin;              // pin to which button is connected
    const uint8_t   longpressLen;     // longpress duration

    ButtonPressEvent getEvent();
};

#endif
