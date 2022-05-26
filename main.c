//------------------------------------------------------------------------------------------
//	*copyright	: L
//	*FileName	: main.c
//	*Author		: Cosmos
//	*Version	: V1.0
//	*Date		: 2018.9.2
//	*Description:
//				  小型四翼飞行器控制板代码
//	*Others		:
//	*Fun List	:
//	*History	:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "include.h"
#include "APP/Task_Scheduler/Task_Schedule.h"
#include "Drv_init/Drv_init.h"
#include "APP/Globle_Var/Globle_Var.h"
#include "Test/Test.h"
#include "Drv_time/Drv_time.h"
#include "Drv_RGB/Drv_RGB.h"
#include "Drv_AT9/Drv_AT9.h"
#include "Drv_PWM/Drv_PWM.h"
//------------------------------------------------------------------------------------------
int main(void)
{
	HSI_SetSysClock(16, 336, 2, 7);
	//初始化所有硬件资源
	while ((fly_flag.ready = all_init()) == 0)
	{
		RGB_Show(0, 0xff, 0);
		delay_ms(100);
	}
	RGB_Red();

	DTInit();
	while (1)
	{
		任务调度
		uint32_t time_1ms = 0, time_2ms = 0, time_5ms = 0, time_10ms = 0;
		uint32_t current_time = SysTick_GetTick();
		//1ms任务
		if (current_time - time_1ms > 1)
		{
			time_1ms = current_time;
			RGB_Red();
		}
		//2ms任务
		else if (current_time - time_2ms > 2)
		{
			time_2ms = current_time;
			AC_Data_Process();		//获得遥控器数据
		}
		//5ms任务
		else if (current_time - time_5ms > 5)
		{
			time_5ms = current_time;
		}
		//10ms任务
		else if (current_time - time_10ms > 10)
		{
			time_10ms = current_time;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------------
