//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Test.c
//	*Author			: Cosmos
//	*Version		: V1.3
//	*Date			: 2018.9.4
//	*Description	: 硬件模块测试
//		
//	*Others			:
//	*Fun List		:
//	*History		: 2018.9.20
//					  PWM,Stick测试成功 
//					  2018.9.24
//					  MPU测试成功
//					  2018.9.26
//					  RGB测试成功
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include <stdio.h>
#include "include.h"
#include "Test/Test.h"
#include "Drv_time/Drv_time.h"
#include "Drv_RGB/Drv_RGB.h"
#include "Drv_AT9/Drv_AT9.h"
#include "Drv_Pwm/Drv_PWM.h"
#include "Drv_uart/Drv_uart.h"
#include "Drv_MPU/Drv_MPU.h"
#include "Drv_I2C/Drv_I2C.h"
#include "Drv_SPI/Drv_SPI.h"
#include "Drv_DMP/inv_mpu.h"
#include "Drv_DMP/inv_mpu_dmp_motion_driver.h"
#include "APP/Globle_Var/Globle_Var.h"
#include "Drv_SPL06/Drv_SPL06.h"
#include "Test/Ano.h"
#define THR_MAX		500
float press, temp;
//------------------------------------------------------------------------------------------
//	*Function	 : Test_time
//	*Descriptiton: 测试系统时钟，在Keil中设端点来调试时间
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_time(void)
{
	//测试延时us
	delay_us(100);
	//测试延时ms
	delay_ms(100);
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_RGB
//	*Descriptiton: 测试信号1和信号0是否正确，测试RGB是否可以点亮，信号1和0在Keil中观察执行时间。
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_RGB(void)
{
	RGB_Init();
	while (1)
	{
		RGB_Rand();
		delay_ms(500);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_Pwm
//	*Descriptiton: 测试PWM波是否可按设定频率，占空比输出
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_Pwm()
{
	DTInit();
	delay_ms(5);
	/*MotorFlash(THR_MAX, THR_MAX, THR_MAX, THR_MAX);*/
	while (1)
	{
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_AT9
//	*Descriptiton: 测试接收器是否能正常接受数据
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_AT9()
{
	AT9_Init();
	while (1)
	{
		/*delay_ms(100);
		USART_SendData(USART2, 'a');*/
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_MPU
//	*Descriptiton: 测试MPU姿态解算模块，发送数据到上位机
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_MPU(void)
{
	float roll = 0, yaw=0, pitch = 0;
	/*USART_Config();*/
	USART_init(115200);
	delay_ms(1);
	MPU_Init();
	while(mpu_dmp_init());
	delay_ms(5);
	while(1) 
	{ 
		mpu_dmp_get_data(&pitch, &roll, &yaw);
		ANO_DT_Send_Status(roll, -pitch, -yaw, 10, 1, 1);
		delay_ms(2);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_I2C
//	*Descriptiton: 测试I2C
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_I2C(void)
{
	I2C_My_Init();
	while (1)
	{
		MPU_Read_Byte(0x75);
		delay_ms(10);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_SPI
//	*Descriptiton: 测试SPI
//	*Parameter	 :
//	*Return		 :	
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_SPI(void)
{
	SPI_My_Init();
	while (1)
	{
		SPI_Write(0xcc, 0xaa);
		delay_ms(5);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_SPL06
//	*Descriptiton: 测试气压计
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_SPL06(void)
{
	SPI_My_Init();
	SPL06_Init();
	while (1)
	{
		SPL06_Update();
		press = SPL06_Get_Pressure();
		temp = SPL06_Get_Temperature();
		delay_ms(20);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_delay
//	*Descriptiton: 测试延时时间
//	*Parameter	 : None	
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void delay_ns(uint8_t ns)
{
	while (ns--);
}
void Test_delay(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	while (1)
	{
		PCout(10) = 1;
		delay_ms(5);
		PCout(10) = 0;
		delay_ms(5);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : Test_uart
//	*Descriptiton: 测试Uart通信
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Test_uart(void)
{
	USART_Config();
	while (1)
	{
		delay_ms(100);
		USART_SendData(USART1, 'a');
	}
}
//------------------------------------------------------------------------------------------
