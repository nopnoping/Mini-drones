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
#include "Drv_DMP/inv_mpu.h"
#include "Drv_DMP/inv_mpu_dmp_motion_driver.h"
#include "Test/Ano.h"
#include "Drv_MPU/Drv_MPU.h"
//------------------------------------------------------------------------------------------
float p, r, y;
short a, b, c;
int main(void)
{
	////初始化所有硬件资源
	while ((fly_flag.ready = all_init()) == 0)
	{
		RGB_Show(0xff,0, 0);
		delay_ms(100);
	}
	RGB_Rand();
	delay_ms(50);
	while (1)
	{
		//任务调度
		static uint32_t time_1ms = 0, time_2ms = 0, time_5ms = 0, time_10ms = 0, time_100ms = 0;
		uint32_t current_time = SysTick_GetTick();
		//1ms任务
		if (current_time - time_1ms > 1)
		{
			time_1ms = current_time;
		/*	RGB_Infor_Show();*/
		}
		//2ms任务
		else if (current_time - time_2ms > 2)
		{
			time_2ms = current_time;
		}
		//5ms任务
		else if (current_time - time_5ms > 5)
		{
			time_5ms = current_time;
			AC_Data_Process();		//获得遥控器数据
		}
		//10ms任务
		else if (current_time - time_10ms > 10)
		{
			time_10ms = current_time;
		}
		else if (current_time - time_100ms > 100)
		{
			time_100ms = current_time;
			mpu_dmp_get_data(&p, &r, &y);
			ANO_DT_Send_Status(r, -p, -y, 10, 1, 1);
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------------
