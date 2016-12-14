#include <Wire.h> 
#include "communication.h"
#include "communication_consts.h"
#include "Params.h"
#include "menus.h"

#define SECONDS_TO_MS(x) ((uint32_t)x * 1000)
#define MINUTES_TO_MS(x) ((uint32_t)x * 60000)
#define MS_TO_SECONDS(x) (x / 1000)
#define MS_TO_MINUTES(x) (x / 60000)
#define PERCENT_TO_VAL(prc, maxVal) (((uint32_t)prc * maxVal) / 100)
#define VAL_TO_PERCENT(val, maxVal) (((uint32_t)val * 100) / maxVal)

Communication commLink;

Communication::Communication() {
  Wire.begin();
}

void Communication::sendWantFeederCommand(bool wantFeeder) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_FEEDER_RUN);
  Wire.write(wantFeeder ? 1 : 0);
  Wire.endTransmission();
}

void Communication::sendLoadCoalCommand(bool doLoad) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_FEEDER_LOAD_COAL);
  Wire.write(doLoad ? 1 : 0);
  Wire.endTransmission();
}

void Communication::sendWorkMode(int8_t mode) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_GENERAL_SET_WORKMODE);
  Wire.write(mode);
  Wire.endTransmission();
  
  Wire.beginTransmission(I2C_DEVICE_PUMPS);
  Wire.write(I2C_CMD_GENERAL_SET_WORKMODE);
  Wire.write(mode);
  Wire.endTransmission();
}
 
uint8_t Communication::requestParamValue(uint8_t deviceId, uint8_t index) {
  Wire.beginTransmission(deviceId);
  Wire.write(I2C_CMD_GENERAL_GET_PARAM);
  Wire.write(index);
  Wire.endTransmission();
  
  Wire.requestFrom((int)deviceId, 1);
  return Wire.read(); 
}

void Communication::sendParamChange(uint8_t deviceId, uint8_t index, uint8_t newValue) {
  Wire.beginTransmission(deviceId);
  Wire.write(I2C_CMD_GENERAL_SET_PARAM);
  Wire.write(index);
  Wire.write(newValue);
  Wire.endTransmission();    
}

void Communication::getGeneralUpdate() {
  //todo:
}

Menu* Communication::getFeederMenu(ExecMenuCallback onActivate) {
  return getMenu(I2C_DEVICE_FEEDER, onActivate);
}

Menu* Communication::getPumpsMenu(ExecMenuCallback onActivate) {
  return getMenu(I2C_DEVICE_PUMPS, onActivate);
}

Menu* Communication::getMenu(int deviceId, ExecMenuCallback onActivate) {
  Wire.beginTransmission(deviceId);
  Wire.write(I2C_CMD_GENERAL_GET_PARAMS_COUNT);
  Wire.endTransmission();
  Wire.requestFrom((int)deviceId, 1);
  int8_t count = Wire.read();

  Param** params = new Param*[count];
  for(int8_t t = 0; t < count; t++) {
    Wire.beginTransmission(deviceId);
    Wire.write(I2C_CMD_GENERAL_GET_PARAM_DESCRIPTION);
    Wire.write(t);
    Wire.endTransmission();
    
    Wire.requestFrom((int)deviceId, 4);
    uint8_t paramType = Wire.read();
    uint8_t paramUnit = (paramType & I2C_PARAM_UNIT_MASK) >> 4;
    paramType &= !I2C_PARAM_UNIT_MASK;
    
    uint8_t minValue = Wire.read();
    uint8_t maxValue = Wire.read();
    uint8_t nameSize = Wire.read();
    
    Wire.beginTransmission(deviceId);
    Wire.write(I2C_CMD_GENERAL_GET_PARAM_NAME);
    Wire.endTransmission();
    char* name = new char[nameSize + 1];
    Wire.requestFrom((int)deviceId, (int)nameSize);
    for(uint8_t y = 0; y < nameSize; y++) {
      name[y] = Wire.read();
    }
    name[nameSize] = 0;

    Param* param;
    if (paramType == I2C_PARAM_TYPE_EXEC) {
      //add remote exec param if needed
      param = NULL;
      
    } else if (paramType == I2C_PARAM_TYPE_VALUE) {
      params[t] = new ValuedParam(name, paramUnit, t, minValue, maxValue);
    }
  }
  return new Menu(deviceId, params, count, onActivate);
}

