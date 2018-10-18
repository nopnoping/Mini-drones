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

#define JUDGE_ENDBYTE(data)				((data==0x04)|(data==0x14)|(data==0x24)|(data==0x34))
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

	/* 配置 USART 为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = AT9_USART_IRQ;
	/* 抢断优先级为 1 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	/* 子优先级为 1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置 NVIC */
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

	/*引脚时钟使能*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/*USART时钟使能*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

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
	/* USART 模式控制：同时使能接收和发送 */
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	/* 完成 USART 初始化配置 */
	USART_Init(USART1, &USART_InitStructure);

	NVIC_Configuration();

	//使能串口接收中断 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能串口
	USART_Cmd(USART1, ENABLE);
}
//------------------------------------------------------------------------------------------
//	*Function	 : AT9_CH_DataProcess
//	*Descriptiton: 将AT9发送的数据，处理为各个通道的数值(每个通道为11位数据)
//	*Parameter	 : data: AT9发送的数据数组
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void AT9_CH_DataProcess(uint16_t *data)
{
	//直接运算
	ch_in[0] = ((((data[1] | data[2] << 8) & 0x07FF))-1000)/4;
	ch_in[1] = ((((data[2] >> 3 | data[3] << 5) & 0x07FF))-1000)/4;
	ch_in[2] = ((((data[3] >> 6 | data[4] << 2 | data[5] << 10) & 0x07FF)) - 1000) / 4;
	ch_in[3] = ((((data[5] >> 1 | data[6] << 7) & 0x07FF)) - 1000) / 4; 
	ch_in[4] = ((((data[6] >> 4 | data[7] << 4) & 0x07FF)) - 1000) / 4;
	ch_in[5] = ((((data[7] >> 7 | data[8] << 1 | data[9] << 9) & 0x07FF)) - 1000) / 4;
	ch_in[6] = ((((data[9] >> 2 | data[10] << 6) & 0x07FF)) - 1000) / 4;
	ch_in[7] = ((((data[10] >> 5 | data[11] << 3) & 0x07FF)) - 1000) / 4;
	//ch_in[8] = ((data[12] | data[13] << 8) & 0x07FF);
	//ch_in[9] = ((data[13] >> 3 | data[14] << 5) & 0x07FF);
	//ch_in[10] = ((data[14] >> 6 | data[15] << 2 | data[16] << 10) & 0x07FF);
	//ch_in[11] = ((data[16] >> 1 | data[17] << 7) & 0x07FF);
	//ch_in[12] = ((data[17] >> 4 | data[18] << 4) & 0x07FF);
	//ch_in[13] = ((data[18] >> 7 | data[19] << 1 | data[20] << 9) & 0x07FF);
	//ch_in[14] = ((data[20] >> 2 | data[21] << 6) & 0x07FF);
	//ch_in[15] = ((data[21] >> 5 | data[22] << 3) & 0x07FF);
}
//------------------------------------------------------------------------------------------
//	*Function	 : AT9_USART_IRQHandler
//	*Descriptiton: USART接受中断，读取遥控器通道数值
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void AT9_USART_IRQHandler(void)
{
	uint16_t data[25];   //数据帧
	static uint8_t index=0;  //下标
	uint8_t temp;   
	static uint8_t flag=0; //标记数据传输开始

	if (USART_GetITStatus(AT9_USART, USART_IT_RXNE) != RESET) {
		temp = USART_ReceiveData(AT9_USART);
		if (flag)
		{
			//一帧中数据
			index++;
			data[index] = temp;
			//传输结束
			if (index == 24)
			{
				flag = 0;		//标志位清零，代表一次传输结束
				index = 0;		//下标清零，为下一次传输做准备
				//判断此帧数据是否正常
				if (data[24] == 0 && data[23] == 0)
				{
					data[23] = 1;
					data[24] = 1;
					AT9_CH_DataProcess(data);
				}
			}
		}
		//判断起始位
		else if (temp == 0x0f && index == 0)
		{
			flag = 1;
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
	if (ch_in[0]<-150&&ch_in[1]>150&&ch_in[2]>150&&ch_in[3]>150)
	{
		if (fly_flag.unclock == 0)
		{
			delay_ms(500);
			if (ch_in[0] < -150 && ch_in[1] >150 && ch_in[2]>150 && ch_in[3]>150)
			{
				fly_flag.unclock = 1;
				RGB_State = 1;
				DTInit();
			}
		}
		else {
			delay_ms(200);
			if (ch_in[0] < -150 && ch_in[1] >150 && ch_in[2]>150 && ch_in[3]>150)
			{
				fly_flag.unclock = 0;
				RGB_State = 3;
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
