#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "include.h"

void SPI_My_Init(void);
void SPI_Write(uint8_t reg, uint8_t data);
void SPI_Read(uint8_t reg, uint8_t len, uint8_t data[]);
#endif // !__DRV_SPI_H__
