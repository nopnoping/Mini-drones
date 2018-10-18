//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_PWM
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.6	 
//	*Description	: PWM1 : PA8  TIME1_CH1			PWM2 : PA9  TIME1_CH2
//					  PWM3 : PE13 TIME1_CH3			PWM4 : PE14 TIME1_CH4
//	*Others			:
//	*Fun List		:
//	*History		: V1.1跟新，更改了油门初值与油门最大值，使得高电平持续时间在1ms到2.5ms之间
//					  误差小于0.1ms
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include"Drv_PWM/Drv_PWM.h"
//------------------------------------------------------------------------------------------
#define THR_MAX			1150
#define THR_BASE		1300
//------------------------------------------------------------------------------------------
//	*Function	 : Pwm_Init
//	*Descriptiton: 初始化四个PWM输出引脚
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void Pwm_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
	//200hz
	TIM_TimeBaseStructure.TIM_Period = 2499;
	TIM_TimeBaseStructure.TIM_Prescaler =70; //400hz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
//------------------------------------------------------------------------------------------
//	*Function	 : DTInit
//	*Descriptiton: 电调初始化，高电平时间在1-2.5ms中有效
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void DTInit(void)
{
	Pwm_Init();
	TIM_SetCompare1(TIM1, THR_BASE);
	TIM_SetCompare2(TIM1, THR_BASE);
	TIM_SetCompare3(TIM1, THR_BASE);
	TIM_SetCompare4(TIM1, THR_BASE);
}
//------------------------------------------------------------------------------------------
//	*Function	 : MotorFlash
//	*Descriptiton: 跟新电机状态
//	*Parameter	 : motor1 : 电机1的转速		motor2 : 电机2的转速
//				   motor3 : 电机3的转速		motor4 : 电机4的转速
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void MotorFlash(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	if (motor1 > THR_MAX) motor1 = THR_MAX;
	if (motor2 > THR_MAX) motor2 = THR_MAX;
	if (motor3 > THR_MAX) motor3 = THR_MAX;
	if (motor4 > THR_MAX) motor4 = THR_MAX;
	if (motor1 <0) motor1 = 0;
	if (motor2 <0) motor2 = 0;
	if (motor3 <0) motor3 = 0;
	if (motor4 <0) motor4 = 0;

	TIM_SetCompare1(TIM1, motor1+THR_BASE);
	TIM_SetCompare2(TIM1, motor2 + THR_BASE);
	TIM_SetCompare3(TIM1, motor3 + THR_BASE);
	TIM_SetCompare4(TIM1, motor4 + THR_BASE);
}
void CloseDT(void)
{
	TIM_SetCompare1(TIM1,0);
	TIM_SetCompare2(TIM1,0);
	TIM_SetCompare3(TIM1,0);
	TIM_SetCompare4(TIM1,0);
}
//------------------------------------------------------------------------------------------
