#include "buttons.h"

Button::Button(uint8_t i, uint8_t p, uint8_t lp)
: id(i), pin(p), longpressLen(lp), wasPressed(0), pressedCounter(0), delayTime(0), lastMillis(0), onShortPress(NULL), onLongPress(NULL) {
  pinMode(pin, INPUT_PULLUP);
#ifdef DEBUG_KEYS
  Serial.print("Configure button id=");
  Serial.print((int)i);
  Serial.print(", pin=");
  Serial.println((int)p);
#endif
}

ButtonPressEvent Button::getEvent() {
  ButtonPressEvent event;
  
  if ((nowPressed == 0) && (wasPressed == 1) ) {
    // handle release event
    if (pressedCounter < longpressLen) {
      event = EV_SHORTPRESS;

    } else {
      event = EV_LONGPRESS;
    }
  } else {
    event = EV_NONE;
  }
  
  return event;
}

ButtonPressEvent Button::update()
{  
  if (delayTime != 0) {
    unsigned long tmp = millis() - lastMillis;
    lastMillis = millis();
    
    if (tmp > delayTime) { 
      delayTime = 0;
    } else {
      delayTime -= tmp;
    }
    if (delayTime != 0) {
      return getEvent();
    }
  }
  delayTime = BUTTON_CHECK_DELAY;
  
  nowPressed = (digitalRead(pin) == HIGH) ? 0 : 1;
  
  ButtonPressEvent event = getEvent();

  if ((event == EV_SHORTPRESS) && (onShortPress != NULL)) {
    onShortPress();
    
  } else if ((event == EV_LONGPRESS) && (onLongPress != NULL)) {
    onLongPress();
  }
  
  // update press running duration
  if (nowPressed) {
    ++pressedCounter;
    if (pressedCounter > longpressLen) {
      pressedCounter = longpressLen + 1;
    }
  } else {
    pressedCounter = 0;
  }
  
  // remember state, and we're done
  wasPressed = nowPressed;
  return event;
}

bool Button::isPressed() {
  return nowPressed == 1 ? true : false;
}

bool Button::isLongPressed() {
  return ((pressedCounter > longpressLen) && (nowPressed == 1)) ? true : false;
}

void Button::setOnLongPressCallback(ButtonCallback callback) {
  onLongPress = callback;
}

void Button::setOnShortPressCallback(ButtonCallback callback) {
  onShortPress = callback;
}


