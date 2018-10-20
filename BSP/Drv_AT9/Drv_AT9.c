//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_AT9
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.4	 
//	*Description	: 初始化接收器，并处理通道值
//		
//	*Others			:
//	*Fun List		:
//	*History		:
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_AT9/Drv_AT9.h"
#include "APP/Globle_Var/Globle_Var.h"
#include "Drv_Pwm/Drv_PWM.h"
#include "Drv_time/Drv_time.h"
uint8_t data[25]; 
//------------------------------------------------------------------------------------------
#define AT9_USART						USART1
#define AT9_USART_CLK					RCC_APB2Periph_USART1
#define AT9_USART_BAUDRATE				99000 //串口波特率

#define AT9_USART_RX_GPIO_PORT			GPIOA
#define AT9_USART_RX_GPIO_CLK			RCC_AHB1Periph_GPIOA
#define AT9_USART_RX_PIN				GPIO_Pin_9
#define AT9_USART_RX_AF					GPIO_AF_USART1
#define AT9_USART_RX_SOURCE				GPIO_PinSource9

#define AT9_USART_TX_GPIO_PORT			GPIOA
#define AT9_USART_TX_GPIO_CLK			RCC_AHB1Periph_GPIOA
#define AT9_USART_TX_PIN				GPIO_Pin_10
#define AT9_USART_TX_AF					GPIO_AF_USART1
#define AT9_USART_TX_SOURCE				GPIO_PinSource10

#define AT9_USART_IRQHandler			USART1_IRQHandler
#define AT9_USART_IRQ					USART1_IRQn

static uint32_t ch_dog = 0;
//------------------------------------------------------------------------------------------
//	*Function	 : ch_feed_dog
//	*Descriptiton: 喂狗
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void ch_feed_dog(void)
{
	ch_dog = 0;
}
//------------------------------------------------------------------------------------------
//	*Function	 : ch_watch_dog
//	*Descriptiton: 开门狗检测
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void ch_watch_dog(uint8_t dT_ms)
{
	uint8_t i = 0;
	if (ch_dog < 500)
	{
		ch_dog += dT_ms;
	}
	else
	{
		for (i = 0; i < 8; i++)
			ch_in[i] = 0;
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : NVIC_Configuration
//	*Descriptiton: 配置中断处理
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	///* 配置 USART 为中断源 */
	//NVIC_InitStructure.NVIC_IRQChannel = AT9_USART_IRQ;
	///* 抢断优先级为 1 */
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	///* 子优先级为 1 */
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	///* 使能中断 */
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	///* 初始化配置 NVIC */
	//NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}
//------------------------------------------------------------------------------------------
//	*Function	 : AT9_Init
//	*Descriptiton: 初始化接收器
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void AT9_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/*引脚时钟使能*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/*USART时钟使能*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/*使能DMA*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	/* 连接 PXx 到 USARTx_Tx*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* GPIO 初始化 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	/* 配置 Rx 引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 配置串 DEBUG_USART 模式 */
	/* 波特率设置： DEBUG_USART_BAUDRATE */
	USART_InitStructure.USART_BaudRate =99000;
	/* 字长(数据位+校验位)： 8 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/* 停止位： 1 个停止位 */
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	/* 校验位选择：偶校验 */
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	/* 硬件流控制：不使用硬件流 */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* USART 模式控制：使能接收 */
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	/* 完成 USART 初始化配置 */
	USART_Init(USART1, &USART_InitStructure);
	////使能串口接收中断 
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能串口
	USART_Cmd(USART1, ENABLE);

	//DMA中断
	NVIC_Configuration();
	/*传输通道*/
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	/*设置DMA源：内存地址&串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) & (USART1->DR);

	/*内存地址(要传输的变量的指针)*/
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;

	/*方向：从外设到内存*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;

	/*传输大小DMA_BufferSize=SENDBUFF_SIZE*/

	DMA_InitStructure.DMA_BufferSize = 25;

	/*外设地址不增*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	/*内存地址自增*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

	/*外设数据单位*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

	/*内存数据单位 8bit*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	/*DMA模式：循环*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	/*优先级：非常高*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

	/*禁止FIFO*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;

	/*突发传输*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA2_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream2, ENABLE);
	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE); 
	USART_DMACmd(USART1,USART_DMAReq_Rx, ENABLE);
	
}
//------------------------------------------------------------------------------------------
//	*Function	 : DMA2_Stream2_IRQHandler
//	*Descriptiton: DMA2中断
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void DMA2_Stream2_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2)==SET)
	{
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_FLAG_TCIF2);
		if (data[0] == 0x0f && data[23] == 0 && data[24] == 0)
		{
			ch_in[0] = ((((data[1] | data[2] << 8) & 0x07FF)) - 1000) / 4;
			ch_in[1] = ((((data[2] >> 3 | data[3] << 5) & 0x07FF)) - 1000) / 4;
			ch_in[2] = ((((data[3] >> 6 | data[4] << 2 | data[5] << 10) & 0x07FF)) - 1000) / 4;
			ch_in[3] = ((((data[5] >> 1 | data[6] << 7) & 0x07FF)) - 1000) / 4;
			ch_in[4] = ((((data[6] >> 4 | data[7] << 4) & 0x07FF)) - 1000) / 4;
			ch_in[5] = ((((data[7] >> 7 | data[8] << 1 | data[9] << 9) & 0x07FF)) - 1000) / 4;
			ch_in[6] = ((((data[9] >> 2 | data[10] << 6) & 0x07FF)) - 1000) / 4;
			ch_in[7] = ((((data[10] >> 5 | data[11] << 3) & 0x07FF)) - 1000) / 4;
			data[0] = 1;
			data[23] = 1;
			data[24] = 1;
			ch_feed_dog();
		}
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : AC_Data_Process
//	*Descriptiton: 处理遥控器信号
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void AC_Data_Process(void)
{
	//油门,姿态数据获取
	RC_Data.thr = ch_in[0] * 1.5;
	RC_Data.pitch = ch_in[1] * 0.08;
	RC_Data.roll = ch_in[2] * 0.08;
	RC_Data.yaw = ch_in[3] * 0.08;
	//解锁
	ch_watch_dog(2);
	if (ch_in[0]<-200&&ch_in[1]>200&&ch_in[2]>200&&ch_in[3]>200)
	{
		if (fly_flag.unclock == 0)
		{
			delay_ms(1000);
			if (ch_in[0]<-200 && ch_in[1]>200 && ch_in[2]>200 && ch_in[3]>200)
			{
				fly_flag.unclock = 1;
				RGB_State = 1;
				DTInit();
			}
		}
		else {
			delay_ms(1000);
			if (ch_in[0]<-200 && ch_in[1]>200 && ch_in[2]>200 && ch_in[3]>200)
			{
				fly_flag.unclock = 0;
				RGB_State = 2;
				CloseDT();
			}
		}
	}
	//定高模式
	if (ch_in[5] > 300)
	{
		fly_flag.height_mod = 1;
		RGB_State = 2;
	}
}
//------------------------------------------------------------------------------------------
