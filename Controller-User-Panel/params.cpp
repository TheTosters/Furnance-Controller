#include "params.h"

#define CHARS_AT_LINE   16
#define SCROLL_DELAY    250

static uint8_t scrollPos = 0;
static uint8_t labelLen = 0;
static unsigned long lastMillis = 0;
static int scrollDelay = 0;
static char* curLabel = NULL;

static char* unitNames[] = {"sek", "min", "stC", "%"};

void getScrollable(char* line, char* label) {
  if (label != curLabel) {
    lastMillis = millis();
    scrollPos = 0;
    labelLen = strlen(label);
    curLabel = label;
    scrollDelay = 2000; //2s before start scrolling
  }
  for(int8_t t = 0; t < CHARS_AT_LINE; t++) {
    int8_t index = t + scrollPos;
    line[t] = index > labelLen ? ' ' : label[index];
  }
  if (labelLen <= CHARS_AT_LINE) {
    return;
  }
  unsigned long tmp = millis() - lastMillis;
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
      scrollDelay = 2000; //2s before start scrolling
    }
  }
}

void setLine(char* dest, char* src) {
  strncpy(dest, src, CHARS_AT_LINE);
  for(int8_t t = strlen(src); t < CHARS_AT_LINE; t++) {
    dest[t] = ' ';
  }
}

Param::Param(char* n) 
: name(n) {
  
}

bool Param::isRemotelyRefreshable() {
  return false;
}

ValuedParam::ValuedParam(char* name, uint8_t _units, uint8_t index, uint8_t min, uint8_t max) 
: Param(name), units(_units), remoteIndex(index), minValue(min), maxValue(max) {
  
}

bool ValuedParam::isRemotelyRefreshable() {
  return true;
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

  snprintf(line2, CHARS_AT_LINE, "%d (%d) %s", value, prevValue, unitNames[units]);
  for(int8_t t = strlen(line2); t < CHARS_AT_LINE; t++) {
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
  strncpy(line2, "<-TAK     NIE->", CHARS_AT_LINE);
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

  char data[CHARS_AT_LINE];
  switch(*workMode) {
    case WorkMode_STOP:
      setLine(line1, "Tryb: STOP");
      snprintf(line2, CHARS_AT_LINE, "CO:%2d,  ->START", *coTemp);
      break;
      
    case WorkMode_RUNNING:
      setLine(line1, "PRACA");
      snprintf(data, CHARS_AT_LINE, "CO:%2d, CW:%2d", *coTemp, *cwTemp);
      setLine(line2, data);
      break;
      
    case WorkMode_STANDBY:
      setLine(line1, "PODTRZYMANIE");
      snprintf(data, CHARS_AT_LINE, "CO:%2d, CW:%2d", *coTemp, *cwTemp);
      setLine(line2, data);
      break;
  }
}

