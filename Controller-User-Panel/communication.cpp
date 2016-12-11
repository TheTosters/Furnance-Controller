#include <Wire.h> 
#include "communication.h"
#include "communication_consts.h"

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
  Wire.write(I2C_CMD_GENERAL_WORKMODE);
  Wire.write(mode);
  Wire.endTransmission();
  
  Wire.beginTransmission(I2C_DEVICE_PUMPS);
  Wire.write(I2C_CMD_GENERAL_WORKMODE);
  Wire.write(mode);
  Wire.endTransmission();
}

void Communication::requestFeederParam(uint8_t index) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_GENERAL_GET_PARAM);
  Wire.write(index);
  Wire.endTransmission();  
}

void Communication::sendFeederParamChange(uint8_t index, uint8_t newValue) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_GENERAL_SET_PARAM);
  Wire.write(index);
  Wire.write(newValue);
  Wire.endTransmission();    
}

void Communication::requestPumpsParam(uint8_t index) {
  Wire.beginTransmission(I2C_DEVICE_PUMPS);
  Wire.write(I2C_CMD_GENERAL_GET_PARAM);
  Wire.write(index);
  Wire.endTransmission();    
}

void Communication::sendPumpsParamChange(uint8_t index, uint8_t newValue) {
  Wire.beginTransmission(I2C_DEVICE_FEEDER);
  Wire.write(I2C_CMD_GENERAL_SET_PARAM);
  Wire.write(index);
  Wire.write(newValue);
  Wire.endTransmission();      
}

void Communication::getGeneralUpdate() {
  
}

