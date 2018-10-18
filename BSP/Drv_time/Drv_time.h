#ifndef __DRV_TIME_H__
#define __DRV_TIME_H__
#include "stm32f4xx.h"

#define TICK_PER_SECOND	1000
#define TICK_US			(1000000/TICK_PER_SECOND)
/* 系统时间（调试用） */
extern volatile uint32_t sysTickUptime;


void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void SysTick_Configuration(void);
uint64_t GetSysTime_us(void);
uint32_t SysTick_GetTick(void);
uint32_t Get_Time(void);
void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q);
#endif // !__DRV_TIME_H__
