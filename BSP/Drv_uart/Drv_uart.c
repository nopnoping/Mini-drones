//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_uart
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.16	 
//	*Description	: uart测试硬件资源
//
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "Drv_uart/Drv_uart.h"
#include <stdio.h>
//引脚定义
/*******************************************************/
#define USARTx USART1
/* 不同的串口挂载的总线不一样，时钟使能函数也不一样，移植时要注意
* 串口 1 和 6 是 RCC_APB2PeriphClockCmd
* 串口 2/3/4/5/7 是 RCC_APB1PeriphClockCmd
*/
#define USARTx_CLK				RCC_APB2Periph_USART1
#define USARTx_CLOCKCMD			RCC_APB2PeriphClockCmd
#define USARTx_BAUDRATE			100000 //串口波特率

#define USARTx_RX_GPIO_PORT		GPIOA
#define USARTx_RX_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define USARTx_RX_PIN			GPIO_Pin_10
#define USARTx_RX_AF			GPIO_AF_USART1
#define USARTx_RX_SOURCE		GPIO_PinSource10

#define USARTx_TX_GPIO_PORT		GPIOA
#define USARTx_TX_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define USARTx_TX_PIN			GPIO_Pin_9
#define USARTx_TX_AF			GPIO_AF_USART1
#define USARTx_TX_SOURCE		GPIO_PinSource9
//------------------------------------------------------------------------------------------
//	*Function	 : USART_init
//	*Descriptiton: UART初始化
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 : 上位机通信串口
//------------------------------------------------------------------------------------------
void USART_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //定义GPIO结构体变量
	USART_InitTypeDef USART_InitStructure;   //定义串口结构体变量

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //使能GPIOC的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);   //使能USART3的时钟

															 //串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); //GPIOC10复用为USART3_tx
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3); //GPIOC11复用为USART3_rx

															   //USART3端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10与GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10，PC11

										   //UART4 初始化设置
	USART_InitStructure.USART_BaudRate = baudrate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//串口接收中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);		//串口空闲中断

	USART_Cmd(USART3, ENABLE);   //使能USART3
}
//------------------------------------------------------------------------------------------
//	*Function	 : USART_Config
//	*Descriptiton: UART初始化
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/*引脚时钟使能*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	/*USART时钟使能*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* 连接 PXx 到 USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	/* 连接 PXx 到 USARTx__Rx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	/* GPIO 初始化 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	/* 配置 Tx 引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* 配置 Rx 引脚为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 配置串 DEBUG_USART 模式 */
	/* 波特率设置： DEBUG_USART_BAUDRATE */
	USART_InitStructure.USART_BaudRate =115200;
	/* 字长(数据位+校验位)： 8 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/* 停止位： 1 个停止位 */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* 校验位选择：偶校验 */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	/* 硬件流控制：不使用硬件流 */
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	/* USART 模式控制：同时使能接收和发送 */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* 完成 USART 初始化配置 */
	USART_Init(USART1, &USART_InitStructure);
	/* 使能串口 */
	USART_Cmd(USART1, ENABLE);
}
//------------------------------------------------------------------------------------------
//	*Function	 : fputc
//	*Descriptiton: 重定向 c 库函数 printf 到串口，重定向后可使用 printf 函数
//	*Parameter	 : 
//	*Return		 : 
//	*Others		 :
//------------------------------------------------------------------------------------------
//重定向 c 库函数 printf 到串口，重定向后可使用 printf 函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(USART1, (uint8_t)ch);
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	return (ch);
}
//重定向 c 库函数 scanf 到串口，重写向后可使用 scanf、 getchar 等函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
//------------------------------------------------------------------------------------------
