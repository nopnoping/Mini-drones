#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include "include.h"

void Pwm_Init(void);
void DTInit(void);
void MotorFlash(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CloseDT(void);
#endif // !__DRV_PWM_H__
