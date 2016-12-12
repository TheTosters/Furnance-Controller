#include "params.h"

#define CHARS_AT_LINE   16
#define SCROLL_DELAY    150

static uint8_t scrollPos = 0;
static uint8_t labelLen = 0;
static long lastMillis = 0;
static int scrollDelay = 0;
static char* curLabel = NULL;

void getScrollable(char* line, char* label) {
  if (label != curLabel) {
    lastMillis = millis();
    scrollPos = 0;
    labelLen = strlen(label);
    curLabel = label;
    scrollDelay = 2000; //2s before start scrolling
  }
  long tmp = millis() - lastMillis;
  lastMillis = millis();
  tmp = tmp > scrollDelay ? scrollDelay : tmp;
  scrollDelay -= tmp;
  
  if (scrollDelay <= 0) {
    //do actual scroll
    scrollDelay = SCROLL_DELAY;
    scrollPos++;
    if (scrollPos == labelLen) {
      scrollDelay = 750;
      
    } else if (scrollPos > labelLen) {
      scrollPos = 0;
    }
  }
  
  for(int t = 0; t < CHARS_AT_LINE; t++) {
    int index = t + scrollPos;
    line[t] = index > labelLen ? ' ' : label[index];
  }
}

void setLine(char* dest, char* src) {
  strncpy(dest, src, CHARS_AT_LINE);
  for(int t = strlen(src); t < CHARS_AT_LINE; t++) {
    dest[t] = ' ';
  }
}

Param::Param(char* n) 
: name(n) {
  
}

ValuedParam::ValuedParam(char* name, uint8_t index, uint8_t min, uint8_t max) 
: Param(name), remoteIndex(index), minValue(min), maxValue(max) {
  
}

void ValuedParam::inc() {
  value ++;
  if (value > maxValue) {
    value = maxValue;
  }
}

void ValuedParam::dec() {
  value --;
  if (value < minValue) {
    value = minValue;
  }
}

void ValuedParam::getLCDLines(char* line1, char* line2) {
  getScrollable(line1, name);

  sprintf(line2, "%d (%d)", value, prevValue);
  for(int t = strlen(line2); t < CHARS_AT_LINE; t++) {
    line2[t] = ' ';
  }
}

ExecParam::ExecParam(char* n, ExecParamCallback ac, ExecParamCallback ec)
: Param(n), actionCallback(ac), exitCallback(ec) {
  
}
    
void ExecParam::inc() {
  actionCallback();
}

void ExecParam::dec() {
  exitCallback();
}

void ExecParam::getLCDLines(char* line1, char* line2) {
  getScrollable(line1, name);
  strcpy(line2, "<-TAK     NIE->");
}

SpecialParam::SpecialParam(uint8_t* cot, uint8_t* cwt, WorkMode* wm, ExecParamCallback ic, ExecParamCallback dc) 
: Param(NULL), coTemp(cot), cwTemp(cwt), workMode(wm), incCallback(ic), decCallback(dc) {
}
    
void SpecialParam::inc() {
  if (incCallback != NULL) {
    incCallback();
  }
}

void SpecialParam::dec() {
  if (decCallback != NULL) {
    decCallback();
  }
}

void SpecialParam::getLCDLines(char* line1, char* line2) {

  char data[16];
  switch(*workMode) {
    case WorkMode_STOP:
      setLine(line1, "Tryb: STOP");
      sprintf(line2, "CO:%2d,  ->START", *coTemp);
      break;
      
    case WorkMode_RUNNING:
      setLine(line1, "PRACA");
      sprintf(data, "CO:%2d, CW:%2d", *coTemp, *cwTemp);
      setLine(line2, data);
      break;
      
    case WorkMode_STANDBY:
      setLine(line1, "PODTRZYMANIE");
      sprintf(data, "CO:%2d, CW:%2d", *coTemp, *cwTemp);
      setLine(line2, data);
      break;
  }
}

