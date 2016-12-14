#include <Wire.h>
#include "communication.h"
#include "logic.h"
#include "communication_consts.h"

//master want info about how many params can be configured in this MCU
#define PARAM_GET_MENU_ITEMS_COUNT 1

//master requested information about max/min/type of param, this is start offset, so if param info is requested
//then in requestedParam will be value (PARAM_INFO_OFFSET + param index)
#define PARAM_INFO_OFFSET 30

//master requested information about textual name of param, this is start offset, so if param info is requested
//then in requestedParam will be value (PARAM_INFO_OFFSET + param index)
#define PARAM_NAME_OFFSET 60

//master requested value of some param, this is start offset, so if param value is requested
//then in requestedParam will be value (PARAM_VALUE_OFFSET + param index)
#define PARAM_VALUE_OFFSET 90

#define SECONDS_TO_MS(x) ((uint32_t)x * 1000)
#define MINUTES_TO_MS(x) ((uint32_t)x * 60000)
#define MS_TO_SECONDS(x) (x / 1000)
#define MS_TO_MINUTES(x) (x / 60000)
#define PERCENT_TO_VAL(prc, maxVal) (((uint32_t)prc * maxVal) / 100)
#define VAL_TO_PERCENT(val, maxVal) (((uint32_t)val * 100) / maxVal)

typedef enum {
  WorkMode_STOP,
  WorkMode_RUNNING,
  WorkMode_STANDBY  
} WorkMode;

//if master i2c want some data, this variable is used to determine what we want!
static uint8_t requestedParam;

static ParamInfo paramsInfo[] = {
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_SECONDS, .minValue=1, .maxValue=255, .name="Czas pracy podajnika"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_SECONDS, .minValue=1, .maxValue=255, .name="Czas powtarzania podawania"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_MINUTES, .minValue=1, .maxValue=255, .name="Czas do przedmuch w podtrzymaniu"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_SECONDS, .minValue=1, .maxValue=100, .name="Czas przedmuchu w podtrzymaniu"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_SECONDS, .minValue=1, .maxValue=255, .name="Czas pracy podajnika w podtrzymaniu"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_DEG_C,   .minValue=40, .maxValue=120, .name="Temperatura alarmu zapalenia podajnika"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_PERCENT, .minValue=0, .maxValue=100, .name="Moc wentylatora"}
};

//forward declarations
void onI2cReceiveEvent(int bytesCount);
void onI2cRequest();

bool requestInRange(uint8_t startRange, uint8_t endRange) {
  return (requestedParam >= startRange) && (requestedParam <= endRange);
}

void initCommunication() {
  Wire.begin(I2C_DEVICE_FEEDER);
  Wire.onRequest(onI2cRequest);
  Wire.onReceive(onI2cReceiveEvent);
}

void onI2cRequest() {
  if (requestedParam == PARAM_GET_MENU_ITEMS_COUNT) {
    Wire.write(7);
    
  } else if (requestInRange(PARAM_INFO_OFFSET, PARAM_INFO_OFFSET + 29)) {
    int index = requestedParam - PARAM_INFO_OFFSET;
    Wire.write(paramsInfo[index].typeAndUnit);
    Wire.write(paramsInfo[index].minValue);
    Wire.write(paramsInfo[index].maxValue);
    Wire.write(strlen(paramsInfo[index].name));
    
  } else if (requestInRange(PARAM_NAME_OFFSET, PARAM_NAME_OFFSET + 29)) {
    int index = requestedParam - PARAM_NAME_OFFSET;
    char* namePtr = paramsInfo[index].name;
    while(*namePtr != NULL) {
      Wire.write(*namePtr);
      namePtr++;
    }
    
  } else if (requestInRange(PARAM_VALUE_OFFSET, PARAM_VALUE_OFFSET + 29)) {
    switch(requestedParam - PARAM_VALUE_OFFSET) {
      case 0:
        Wire.write( MS_TO_SECONDS(workConfig.feederRunTime) );
        break;
      case 1:
        Wire.write( MS_TO_SECONDS(workConfig.feederRepeatTime) );
        break;
      case 2:
        Wire.write( MS_TO_MINUTES( workConfig.keepAliveRepeatTime ) );
        break;
      case 3:
        Wire.write( MS_TO_SECONDS(workConfig.keepAliveFanTime) );
        break;
      case 4:
        Wire.write( MS_TO_SECONDS(workConfig.keepAliveFeederTime) );
        break;
      case 5:
        Wire.write(workConfig.feederTempCritical);
        break;
      case 6:
        Wire.write( VAL_TO_PERCENT(workConfig.fanPower, 128) );
        break;
      default:
        Wire.write(0);
        break;
    }
  }
}

void changeParamValue(uint8_t paramIndex, uint8_t value) {
  switch(paramIndex) {
    case 0:
        workConfig.feederRunTime = SECONDS_TO_MS(value);
        break;
      case 1:
        workConfig.feederRepeatTime = SECONDS_TO_MS(value);
        break;
      case 2:
        workConfig.keepAliveRepeatTime = MINUTES_TO_MS(value);
        break;
      case 3:
        workConfig.keepAliveFanTime = SECONDS_TO_MS(value);
        break;
      case 4:
        workConfig.keepAliveFeederTime = SECONDS_TO_MS(value);
        break;
      case 5:
        workConfig.feederTempCritical = value;
        break;
      case 6:
        workConfig.fanPower = PERCENT_TO_VAL(value,128);
        workConfig.fanPower = workConfig.fanPower > 128 ? 128 : workConfig.fanPower;
        workConfig.fanPower = workConfig.fanPower < 0 ? 0 : workConfig.fanPower;
        break;

      default:
        break;
    }
}

void onI2cReceiveEvent(int bytesCount) {

  uint8_t cmd = Wire.read();
  uint8_t cmdValue = (bytesCount == 2) ? Wire.read() : 0;

  switch(cmd) {
    case I2C_CMD_GENERAL_SET_WORKMODE:
      startedMode = cmdValue != WorkMode_STOP;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM:
      requestedParam = PARAM_VALUE_OFFSET + cmdValue;
      break;
    
    case I2C_CMD_GENERAL_SET_PARAM:
      changeParamValue(cmdValue, Wire.read());
      break;
    
    case I2C_CMD_GENERAL_GET_PARAMS_COUNT:
      requestedParam = PARAM_GET_MENU_ITEMS_COUNT;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_DESCRIPTION:
      requestedParam = PARAM_INFO_OFFSET + cmdValue;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_NAME:
      requestedParam = PARAM_NAME_OFFSET + cmdValue;
      break;
    
  }
}
