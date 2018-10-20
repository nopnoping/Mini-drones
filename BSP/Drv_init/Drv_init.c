//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_init
//	*Author			: Cosmos
//	*Version		: v1.0
//	*Date			: 2018.9.3
//	*Description	: 对硬件资源初始化
//		
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_init/Drv_init.h"
#include "Drv_time/Drv_time.h"
#include "Drv_RGB/Drv_RGB.h"
#include "Drv_AT9/Drv_AT9.h"
#include "Drv_MPU/Drv_MPU.h"
#include "APP/Control/Control.h"
#include "Drv_PWM/Drv_PWM.h"
#include "Drv_SPL06/Drv_SPL06.h"
#include "Drv_SPI/Drv_SPI.h"
#include "Drv_I2C/Drv_I2C.h"
#include "Drv_uart/Drv_uart.h"
#include "Drv_DMP/inv_mpu.h"
#include "Drv_DMP/inv_mpu_dmp_motion_driver.h"
#include "Drv_DMP/dmpmap.h"
//------------------------------------------------------------------------------------------
//	*Function	 :	all_init
//	*Descriptiton:	硬件资源初始化
//	*Parameter	 :	None
//	*Return		 :	初始化成功返回1
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t all_init(void)
{
	//滴答定时器
	SysTick_Configuration();
	////RGB灯
	RGB_Init();
	//I2C，为MPU提供通信
	I2C_My_Init();
	//六轴MPU
	if (MPU_Init())
		return 0;
	if (mpu_dmp_init())
		return 0;
	//遥控器信号接收器
	AT9_Init();
	//Pwm初始化
	Pwm_Init();
	//匿名上位机
	USART_init(500000);
	//SPI
	//SPI_My_Init();
	//气压计
	//SPL06_Init();
	//PID初始化
	/*PID_Init();*/
	return 1;
}
//------------------------------------------------------------------------------------------
