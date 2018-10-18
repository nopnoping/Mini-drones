#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "include.h"

void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_Wait_Ack(void);
void I2C_Ack(void);
void I2C_My_Init(void);
void I2C_Send_Byte(uint8_t data);
uint8_t I2C_Read_Byte(uint8_t ack);
//uint8_t I2C_Write_Bytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
//uint8_t I2C_Read_Bytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
#endif // !__DRV_I2C_H__
