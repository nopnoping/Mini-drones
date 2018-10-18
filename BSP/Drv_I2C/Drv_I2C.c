//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		:
//	*Author			: Cosmos
//	*Version		: 
//	*Date			:	 
//	*Description	: SCL : PB0
//					  SDA : PC5
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_I2C/Drv_I2C.h"
#include "Drv_time/Drv_time.h"
#include "APP/Globle_Var/Globle_Var.h"
//------------------------------------------------------------------------------------------
#define SCL_GPIO			GPIOB
#define SCL_Periph_CLK		RCC_AHB1Periph_GPIOB
#define SCL_Pin				GPIO_Pin_0

#define SDA_GPIO			GPIOC
#define SDA_Periph_CLK		RCC_AHB1Periph_GPIOC
#define SDA_Pin				GPIO_Pin_5

#define SCL_H				(PBout(0)=1)
#define SCL_L				(PBout(0)=0)
#define SDA_H				(PCout(5)=1)
#define	SDA_L				(PCout(5)=0)

//#define SDA_IN()()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	    9
//#define SDA_OUT()() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;}
#define SDA_OUT()			{SDA_GPIO->MODER &= ~(3 << (2 * 5)); SDA_GPIO->MODER|=(1<<5*2);}
#define SDA_IN()			{SDA_GPIO->MODER &= ~(3 << (2 * 5)); SDA_GPIO->MODER|=(0<<5*2);}
#define SDA_Read			(PCin(5))
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_My_Init
//	*Descriptiton: 初始化I2C引脚
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void I2C_My_Init()
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(SCL_Periph_CLK | SDA_Periph_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = SCL_Pin;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(SCL_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = SDA_Pin;
	GPIO_Init(SDA_GPIO, &GPIO_InitStruct);
	/*SDA_H;
	SCL_H;*/
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Start
//	*Descriptiton: I2C起始信号
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void I2C_Start(void)
{
	SDA_OUT();
	SDA_H;
	SCL_H;
	delay_us(4);
	SDA_L;
	delay_us(4);
	SCL_L;
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Stop
//	*Descriptiton: I2C通信停止
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void I2C_Stop(void)
{
	SDA_OUT();
	SCL_L;
	SDA_L;
	delay_us(4);
	SCL_H;
	delay_us(2);
	SDA_H;
	delay_us(4);
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Wait_Ack
//	*Descriptiton: 等待从机应答信号
//	*Parameter	 : None
//	*Return		 : 0 : 接受应答成功		1 : 接受应答失败
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t I2C_Wait_Ack(void)
{
	uint8_t errTime = 0;
	SDA_IN();
	SDA_H;
	delay_us(1);
	SCL_H;
	while (SDA_Read)
	{
		errTime++;
		if (errTime > 250)
		{
			I2C_Stop();
			return 1;
		}
	}
	SCL_L;
	return 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Ack
//	*Descriptiton: 产生应答信号
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void I2C_Ack(void)
{
	SCL_L;
	SDA_OUT();
	SDA_L;
	delay_us(1);
	SCL_H;
	delay_us(2);
	SCL_L;
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_No_Ack
//	*Descriptiton: 不产生应答信号
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void I2C_No_Ack(void)
{
	SCL_L;
	SDA_OUT();
	SDA_H;
	delay_us(2);
	SCL_H;
	delay_us(2);
	SCL_L;
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Send_One_Byte
//	*Descriptiton: 发送一个字节的数据
//	*Parameter	 : data : 所需要发送的数据
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void I2C_Send_Byte(uint8_t data)
{
	uint8_t i = 0;
	SDA_OUT();
	SCL_L;
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			SDA_H;
		else
			SDA_L;
		data <<= 1;
		delay_us(4);
		SCL_H;
		delay_us(2);
		SCL_L;
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Read_One_Byte
//	*Descriptiton: 读一个字节
//	*Parameter	 : None
//	*Return		 : 一个字节的数据
//	*Others		 :
//------------------------------------------------------------------------------------------
uint8_t I2C_Read_Byte(uint8_t ack)
{
	uint8_t i = 0,rec=0;
	SDA_IN();
	for (i = 0; i < 8; i++)
	{
		SCL_L;
		delay_us(4);
		SCL_H;
		rec <<= 1;
		if (SDA_Read)
			rec++;
		delay_us(4);
	}
	if (ack)
		I2C_Ack();
	else
		I2C_No_Ack();
	return rec;
}
//------------------------------------------------------------------------------------------
//	*Function	 : I2C_Write_Bytes
//	*Descriptiton: 向寄存器中写入配置
//	*Parameter	 : addr : 从机地址		reg  : 寄存器地址
//				   len	: 所发数据位数	data : 所发数据
//	*Return		 : 0	: 写寄存器成功	1	 : 写寄存器失败
//	*Others		 :
//------------------------------------------------------------------------------------------
//uint8_t I2C_Write_Bytes(uint8_t addr,uint8_t reg, uint8_t len, uint8_t *data)
//{
//	uint8_t i = 0;
//	I2C_Start();
//	I2C_Send_One_Byte(addr << 1);
//	if (I2C_Wait_Ack())
//	{
//		I2C_Stop();
//		return 1;
//	}
//	I2C_Send_One_Byte(reg);
//	I2C_Wait_Ack();
//	for (i = 0; i < len; i++)
//	{
//		I2C_Send_One_Byte(data[i]);
//		if (I2C_Wait_Ack())
//		{
//			I2C_Stop();
//			return 1;
//		}
//	}
//	I2C_Stop();
//	return 0;
//}
////------------------------------------------------------------------------------------------
////	*Function	 : I2C_Read_Bytes
////	*Descriptiton: 读取寄存器的数值
////	*Parameter	 : addr : 从机地址		reg  : 寄存器地址
////				   len	: 读取数据位数	data : 读取数据存放数组
////	*Return		 : 0	: 读取数据成功	1	 : 读取数据失败
////	*Others		 :
////------------------------------------------------------------------------------------------
//uint8_t I2C_Read_Bytes(uint8_t addr,uint8_t reg, uint8_t len, uint8_t *data)
//{
//	I2C_Start();
//	I2C_Send_One_Byte(addr<<1);
//	if (I2C_Wait_Ack())
//	{
//		I2C_Stop();
//		return 1;
//	}
//	I2C_Send_One_Byte(reg);
//	I2C_Wait_Ack();
//	I2C_Start();
//	I2C_Send_One_Byte((addr << 1) + 1);
//	I2C_Wait_Ack();
//	while (len)
//	{
//		if (len == 1)
//			*data = I2C_Read_One_Byte(0);
//		else
//			*data = I2C_Read_One_Byte(1);
//		data++;
//		len--;
//	}
//	I2C_Stop();
//	return 0;
//}
//------------------------------------------------------------------------------------------
