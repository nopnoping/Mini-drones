#ifndef __DRV_LED_H__
#define __DRV_LED_H__
#include "include.h"

void RGB_Init(void);
void send_high_bit(void);
void send_low_bit(void);
void RGB_Colorset(uint8_t red, uint8_t green, uint8_t blue);
void RGB_Infor_Show(void);
void RGB_Rand(void);
void RGB_Show(uint8_t red, uint8_t green, uint8_t blue);
#endif // !__DRV_LED_H__
