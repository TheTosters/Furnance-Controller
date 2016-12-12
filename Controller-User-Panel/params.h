#ifndef __UI_H__
#define __UI_H__

#include <Arduino.h>
class Param {
  public:
    Param(char* name);
    virtual void  inc() = 0;
    virtual void  dec() = 0;
    virtual void  getLCDLines(char* line1, char* line2) = 0;
  protected:
      char*       name;
};

class ValuedParam : public Param {
  public:
    ValuedParam(char* name, uint8_t remoteIndex, uint8_t min, uint8_t max);
    
    void    inc();
    void    dec();
    void    getLCDLines(char* line1, char* line2);

    inline void setValue(uint8_t val) { value = val; prevValue = val;}
    inline uint8_t getValue() {return value;}
    inline uint8_t getRemoteIndex() {return remoteIndex;}
  private:
    uint8_t minValue, maxValue;
    uint8_t value;
    uint8_t remoteIndex;
    uint8_t prevValue;
};

typedef void (*ExecParamCallback)();

class ExecParam : public Param {
  public:
    ExecParam(char* name, ExecParamCallback actionCallback, ExecParamCallback exitCallback);
    
    void    inc();
    void    dec();
    void    getLCDLines(char* line1, char* line2);
  private:
    ExecParamCallback actionCallback;
    ExecParamCallback exitCallback;
};

typedef enum {
  WorkMode_STOP,
  WorkMode_RUNNING,
  WorkMode_STANDBY  
} WorkMode;

class SpecialParam : public Param {
  public:
    SpecialParam(uint8_t* coTemp, uint8_t* cwTemp, WorkMode* workMode, ExecParamCallback incCallback, ExecParamCallback decCallback);
    
    void    inc();
    void    dec();
    void    getLCDLines(char* line1, char* line2);
  private:
    uint8_t*            coTemp;
    uint8_t*            cwTemp;
    WorkMode*           workMode;
    ExecParamCallback   incCallback;
    ExecParamCallback   decCallback;
};

#endif
