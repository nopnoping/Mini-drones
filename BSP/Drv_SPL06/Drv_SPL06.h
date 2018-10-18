#ifndef __DRV_SPL06_H__
#define __DRV_SPL06_H__

#include "include.h"

void SPL06_Init(void);
void SPL06_Update(void);
float SPL06_Get_Temperature(void);
float SPL06_Get_Pressure(void);
#endif // !__DRV_SPL06_H__
