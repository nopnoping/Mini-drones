//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Task_Schedule
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.2
//	*Description	: 初始化线程任务，执行任务调度
//	
//	*Others			:	
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include <stdio.h>
#include "Task_Scheduler/Task_Schedule.h"
#include "Drv_time/Drv_time.h"
#include "Drv_RGB/Drv_RGB.h"
#include "Drv_AT9/Drv_AT9.h"
#include "APP/Globle_Var/Globle_Var.h"
#include "Drv_MPU/Drv_MPU.h"
#include "APP/Control/Control.h"
#include "Drv_RGB/Drv_RGB.h"
#include "Drv_SPL06/Drv_SPL06.h"
//------------------------------------------------------------------------------------------
//	*Function	 :	Scheduler_Run()
//	*Descriptiton:	根据滴答定时器的定时，判断执行哪个线程任务
//	*Parameter	 :	None
//	*Return		 :	None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Scheduler_Run(void)
{
	uint32_t time_1ms = 0,time_2ms=0,time_5ms=0,time_10ms=0;
	uint32_t current_time = SysTick_GetTick();
	//1ms任务
	if (current_time - time_1ms > 1)
	{
		time_1ms = current_time;
		RGB_Red();
	}
	//2ms任务
	else if(current_time - time_2ms > 2)
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
	//for (index = 0; index < 8; index++)
	//{
	//	////获取系统当前时间，单位MS
	//	//uint32_t tnow = SysTick_GetTick();
	//	///*进行判断，如果当前时间减去上一次执行的时间，大于等于该线程的执行周期，则执行线程*/
	//	//if (tnow - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
	//	//{

	//	//	//更新线程的执行时间，用于下一次判断
	//	//	sched_tasks[index].last_run = tnow;
	//	//	//执行线程函数，使用的是函数指针
	//	//	sched_tasks[index].task_func();
	//	//}
	//}


}
//------------------------------------------------------------------------------------------

