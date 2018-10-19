#ifndef __DRV_AT9_H__
#define __DRV_AT9_H__

#include "include.h"

void AT9_Init(void);
void AC_Data_Process(void);
void AT9_CH_DataProcess(uint16_t *data);
#endif // !__DRV_AT9_H__
