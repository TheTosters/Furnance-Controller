#ifndef __UI_H__
#define __UI_H__

#include <Arduino.h>

#define DEBUG_PARAMS 1

class Param {
  public:
    Param(char* name);
    ~Param();
    virtual void  inc() = 0;
    virtual void  dec() = 0;
    virtual void  getLCDLines(char* line1, char* line2) = 0;
    virtual bool  isRemotelyRefreshable();
    virtual void  restorePrevious();
  protected:
      char*       name;
};

class ValuedParam : public Param {
  public:
    ValuedParam(char* name, uint8_t units, uint8_t min, uint8_t max);

    virtual void inc() override;
    virtual void dec() override;
    virtual void getLCDLines(char* line1, char* line2) override;
    virtual bool isRemotelyRefreshable() override;
    virtual void restorePrevious() override;

    inline void setValue(uint8_t val) { value = val; prevValue = val;}
    inline uint8_t getValue() {return value;}
    inline uint8_t getUnits() {return units;}
  private:
    uint8_t minValue, maxValue;
    uint8_t value;
    uint8_t prevValue;
    uint8_t units;
};

typedef void (*ExecParamCallback)();

class ExecParam : public Param {
  public:
    ExecParam(char* name, ExecParamCallback actionCallback, ExecParamCallback exitCallback);
    
    virtual void inc() override;
    virtual void dec() override;
    virtual void getLCDLines(char* line1, char* line2) override;
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
    
    virtual void inc() override;
    virtual void dec() override;
    virtual void getLCDLines(char* line1, char* line2) override;
  private:
    uint8_t*            coTemp;
    uint8_t*            cwTemp;
    WorkMode*           workMode;
    ExecParamCallback   incCallback;
    ExecParamCallback   decCallback;
};

extern bool isParamEditing;
void clearRenderCache();
#endif

