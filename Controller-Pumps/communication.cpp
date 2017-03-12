#include <Wire.h>
#include "communication.h"
#include "logic.h"
#include "communication_consts.h"
#include "temperature.h"

#define DEBUG_COMMUNICATION 1

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

//master requested CO & CW temperatures
#define PARAM_CO_CW 120

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

//WARNING: Wire libreary limit is 32b, so length of name can't exceed it!
static ParamInfo paramsInfo[] = {
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_DEG_C, .minValue=20, .maxValue=60, .name="Min.temp. wody CW"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_DEG_C, .minValue=20, .maxValue=60, .name="Max.temp. wody CW"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_DEG_C, .minValue=30, .maxValue=85, .name="Min.temp. kotla CO"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_DEG_C, .minValue=30, .maxValue=85, .name="Max.temp. kotla CO"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_MINUTES, .minValue=1, .maxValue=255, .name="Cykliczne zalaczanie pomp"},
  {.typeAndUnit = I2C_PARAM_TYPE_VALUE | I2C_PARAM_UNIT_SECONDS, .minValue=2, .maxValue=255, .name="Czas pracy pomp w cykl.zal."}  
};

//forward declarations
void onI2cReceiveEvent(int bytesCount);
void onI2cRequest();

uint8_t applyBoundaries(uint8_t value, uint8_t paramIndex) {
#ifdef DEBUG_COMMUNICATION
  Serial.print("applyBoundaries value:");
  Serial.print(value);
  Serial.print(" min:");
  Serial.print(paramsInfo[paramIndex].minValue);
  Serial.print(" max:");
  Serial.println(paramsInfo[paramIndex].maxValue);
#endif

  value = value < paramsInfo[paramIndex].minValue ? paramsInfo[paramIndex].minValue : value;
  value = value > paramsInfo[paramIndex].maxValue ? paramsInfo[paramIndex].maxValue : value;

  return value;
}

bool requestInRange(uint8_t startRange, uint8_t endRange) {
  return (requestedParam >= startRange) && (requestedParam <= endRange);
}

void initCommunication() {
  Wire.begin(I2C_DEVICE_PUMPS);
  Wire.onRequest(onI2cRequest);
  Wire.onReceive(onI2cReceiveEvent);
}

void onI2cRequest() {
  if (requestedParam == PARAM_CO_CW ) {
    uint8_t tmp = (uint8_t)temperature[TEMP_CO];
    Wire.write(tmp);
    tmp = (uint8_t)temperature[TEMP_CW];
    Wire.write(tmp);

  } else if (requestedParam == PARAM_GET_MENU_ITEMS_COUNT) {
    Wire.write(6);
    
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
        Wire.write( logicConfig.minCWTemp );
        break;
      case 1:
        Wire.write( logicConfig.maxCWTemp );
        break;
      case 2:
        Wire.write( logicConfig.minCOTemp );
        break;
      case 3:
        Wire.write( logicConfig.maxCOTemp );
        break;
      case 4:
        Wire.write( MS_TO_MINUTES(logicConfig.heaterPumpCyclicRunDelay) );
        break;
      case 5:
        Wire.write( MS_TO_SECONDS(logicConfig.heaterPumpCyclicRunDuration) );
        break;
      default:
        Wire.write(0);
        break;
    }
    
  }
}

void changeParamValue(uint8_t paramIndex, uint8_t value) {
  value = applyBoundaries(value, paramIndex);
  switch(paramIndex) {
    case 0:
      logicConfig.minCWTemp = value;
      break;
    case 1:
      logicConfig.maxCWTemp = value;
      break;
    case 2:
      logicConfig.minCOTemp = value;
      break;
    case 3:
      logicConfig.maxCOTemp = value;
      break;
    case 4:
      logicConfig.heaterPumpCyclicRunDelay = MINUTES_TO_MS(value);
      break;
    case 5:
      logicConfig.heaterPumpCyclicRunDuration = SECONDS_TO_MS(value);
      break;

    default:
      break;
  }
}

void onI2cReceiveEvent(int bytesCount) {

  uint8_t cmd = Wire.read();
  uint8_t cmdIndex = (bytesCount >= 2) ? Wire.read() : 0;
  uint8_t cmdValue = (bytesCount == 3) ? Wire.read() : 0;
#ifdef DEBUG_COMMUNICATION
  Serial.print("onI2cReceiveEvent cmd:");
  Serial.print(cmd);
  Serial.print(" index:");
  Serial.print(cmdIndex);
  Serial.print(" value:");
  Serial.println(cmdValue);
#endif

  switch(cmd) {
    case I2C_CMD_GENERAL_SET_WORKMODE:
      startedMode = cmdIndex != WorkMode_STOP;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM:
      requestedParam = PARAM_VALUE_OFFSET + cmdIndex;
      break;
    
    case I2C_CMD_GENERAL_SET_PARAM:
      changeParamValue(cmdIndex, cmdValue);
      break;
    
    case I2C_CMD_GENERAL_GET_PARAMS_COUNT:
      requestedParam = PARAM_GET_MENU_ITEMS_COUNT;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_DESCRIPTION:
      requestedParam = PARAM_INFO_OFFSET + cmdIndex;
      break;
    
    case I2C_CMD_GENERAL_GET_PARAM_NAME:
      requestedParam = PARAM_NAME_OFFSET + cmdIndex;
      break;

    case I2C_CMD_GET_CO_CW_TEMP:
      requestedParam = PARAM_CO_CW;
      break;

    default:
#ifdef DEBUG_COMMUNICATION
      Serial.println("Unknown command type");
#endif
  }
}
