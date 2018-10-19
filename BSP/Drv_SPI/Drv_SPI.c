//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_SPI
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.14
//	*Description	: CS   : PA4		SCK  : PA5
//					  MISO : PA6		MOSI : PA7
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_SPI/Drv_SPI.h"
#include "Drv_time/Drv_time.h"
#include "APP/Globle_Var/Globle_Var.h"
//------------------------------------------------------------------------------------------
#define SPI_RCC_CLK			RCC_AHB1Periph_GPIOA

#define CS_GPIO				GPIOA
#define CS_Pin				GPIO_Pin_4

#define SCK_GPIO			GPIOA
#define SCK_Pin				GPIO_Pin_5

#define MISO_GPIO			GPIOA
#define MISO_Pin			GPIO_Pin_6

#define MOSI_GPIO			GPIOA
#define MOSI_Pin			GPIO_Pin_7

//#define CS_H				CS_GPIO->BSRRL=CS_Pin
//#define CS_L				CS_GPIO->BSRRH=CS_Pin
#define CS_H				(PAout(4)=1)
#define CS_L				(PAout(4)=0)

//#define SCK_H				SCK_GPIO->BSRRL=SCK_Pin
//#define SCK_L				SCK_GPIO->BSRRH=SCK_Pin
#define SCK_H				(PAout(5)=1)
#define SCK_L				(PAout(5)=0)

//#define MOSI_H				MOSI_GPIO->BSRRL=MOSI_Pin
//#define MOSI_L				MOSI_GPIO->BSRRH=MOSI_Pin
#define MOSI_H				(PAout(7)=1)
#define MOSI_L				(PAout(7)=0)

#define MISO_Read			(PAin(6))
//------------------------------------------------------------------------------------------
//	*Function	 : SPI_My_Init
//	*Descriptiton: SPI通信引脚初始化
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void SPI_My_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(SPI_RCC_CLK, ENABLE);
	//时钟和写数据引脚
	GPIO_InitStruct.GPIO_Pin = CS_Pin|MOSI_Pin|SCK_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//读数据引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = MISO_Pin;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}
//------------------------------------------------------------------------------------------
//	*Function	 : SPI_Write_One_Byte
//	*Descriptiton: 发送一个字节的数据
//	*Parameter	 : data : 所发数据
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void SPI_Write_One_Byte(uint8_t data)
{
	uint8_t i = 8;
	SCK_L;
	delay_us(1);
	while (i--)
	{
		if (data & 0x80)
			MOSI_H;
		else
			MOSI_L;
		data <<= 1;
		delay_us(4);
		SCK_H;
		delay_us(4);
		SCK_L;
	}
	SCK_L;
}
//------------------------------------------------------------------------------------------
//	*Function	 : SPI_Read_One_Byte
//	*Descriptiton: 读取一个字节的数据
//	*Parameter	 : *data : 数据储存指针
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void SPI_Read_One_Byte(uint8_t *data)
{
	uint8_t i = 8;
	SCK_L;
	delay_us(1);
	while (i--)
	{
		SCK_H;
		delay_us(4);
		if (MISO_Read)
			*data |= 1;
		SCK_L;
		*data <<= 1;
		delay_us(4);
	}
	SCK_L;
}
//------------------------------------------------------------------------------------------
//	*Function	 : SPI_Write
//	*Descriptiton: 向寄存器写数据
//	*Parameter	 : reg : 寄存器地址	 data : 写入数据
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void SPI_Write(uint8_t reg, uint8_t data)
{
	CS_L;
	SPI_Write_One_Byte(reg&0x7f);
	delay_us(2);
	SPI_Write_One_Byte(data);
	CS_H;
}
//------------------------------------------------------------------------------------------
//	*Function	 : SPI_Read
//	*Descriptiton: 读取寄存器的数据
//	*Parameter	 : reg : 寄存器的地址		*data : 储存数据
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void SPI_Read(uint8_t reg,uint8_t len, uint8_t data[])
{
	CS_L;
	SPI_Write_One_Byte(reg|0x80);
	delay_us(2);
	while (len--)
	{
		SPI_Read_One_Byte(data);
		data++;
	}
	CS_H;
}
//------------------------------------------------------------------------------------------
