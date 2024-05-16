#ifndef _I2C_H_
#define _I2C_H_

#include "stm32f4xx.h"
#include "sys.h"

#define SCL PBout(8)
#define SDAOUT PBout(9)
#define SDAIN PBin(9)

void init_i2c();
void i2cMode(GPIOMode_TypeDef GPIO_Mode);
void i2cStart();
void i2cStop();
void i2cSend(u8 data);
u8 waitAck();
u8 i2cRead();
void sendAck();

#endif