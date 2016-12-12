#ifndef __COMMUNICATION_CONSTS_H__
#define __COMMUNICATION_CONSTS_H__

#define I2C_DEVICE_FEEDER   1
#define I2C_DEVICE_PUMPS    1

#define I2C_CMD_FEEDER_RUN        1
#define I2C_CMD_FEEDER_LOAD_COAL  2

#define I2C_CMD_GENERAL_WORKMODE                20
#define I2C_CMD_GENERAL_GET_PARAM               21
#define I2C_CMD_GENERAL_SET_PARAM               22
#define I2C_CMD_GENERAL_GET_PARAMS_COUNT        23
#define I2C_CMD_GENERAL_GET_PARAM_DESCRIPTION   24
#define I2C_CMD_GENERAL_GET_PARAM_NAME          25

#define I2C_PARAM_TYPE_EXEC   1
#define I2C_PARAM_TYPE_VALUE  2

#endif
