//------------------------------------------------------------------------------------------
//	*copyright		: L
//	*FileName		: Drv_time
//	*Author			: Cosmos
//	*Version		: V1.0
//	*Date			: 2018.9.2
//	*Description	: 系统时钟初始化。
//					  获取当前系统执行时间。
//					  配置高精度us，ms级的延迟
//	*Others			:
//	*Fun List		:
//	*History		:
//------------------------------------------------------------------------------------------
//Includes
//------------------------------------------------------------------------------------------
#include "./BSP/Drv_time/Drv_time.h"
//------------------------------------------------------------------------------------------
volatile uint32_t sysTickUptime = 0;
//------------------------------------------------------------------------------------------
//	*Function	 : SysTick_Configuration
//	*Descriptiton: 滴答定时器初始化
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void SysTick_Configuration()
{
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		while (1);
	}
}
//------------------------------------------------------------------------------------------
//	*Function	 : SysTick_Handler
//	*Descriptiton: 定时器中断，每1ms进入一次，ms计时变量加1
//	*Parameter	 : None
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
	sysTickUptime++;
}
//------------------------------------------------------------------------------------------
//	*Function	 : SysTick_GetTick
//	*Descriptiton: 获取当前系统执行时间(单位ms)
//	*Parameter	 : None
//	*Return		 : 当前系统时间(单位ms)
//	*Others		 :
//------------------------------------------------------------------------------------------
uint32_t SysTick_GetTick(void)
{
	return sysTickUptime;
}
//------------------------------------------------------------------------------------------
//	*Function	 : GetSysTime_us
//	*Descriptiton: 获取当前系统执行时间(单位us)
//	*Parameter	 : None
//	*Return		 : 当前系统时间(单位us)
//	*Others		 :
//------------------------------------------------------------------------------------------
uint64_t GetSysTime_us(void)
{
	volatile uint32_t ms;
	uint32_t value;
	ms = sysTickUptime;
	value = ms*TICK_US+(SysTick->LOAD-SysTick->VAL)*TICK_US/SysTick->LOAD;
	return value;
}
//------------------------------------------------------------------------------------------
//	*Function	 : delay_us
//	*Descriptiton: us高精度延时
//	*Parameter	 : us:所需延时时间(1=1us)
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void delay_us(uint32_t us)
{
	uint64_t now = GetSysTime_us();
	while ((GetSysTime_us() - now)< us);
}
//------------------------------------------------------------------------------------------
//	*Function	 : delay_ms
//	*Descriptiton: ms高精度延时
//	*Parameter	 : ms:所需延时时间(1=1ms)
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
void delay_ms(uint32_t ms)
{
	delay_us(ms * 1000);
}
//------------------------------------------------------------------------------------------
//	*Function	 : delay_ms
//	*Descriptiton: ms高精度延时
//	*Parameter	 : ms:所需延时时间(1=1ms)
//	*Return		 : None
//	*Others		 :
//------------------------------------------------------------------------------------------
uint32_t Get_Time(void)
{
	return sysTickUptime;
}
//------------------------------------------------------------------------------------------
void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)
{
	__IO uint32_t HSIStartUpStatus = 0;
	// 把 RCC 外设初始化成复位状态
	RCC_DeInit();
	//使能 HSI, HSI=16M
	RCC_HSICmd(ENABLE);
	// 等待 HSI 就绪
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	// 只有 HSI 就绪之后则继续往下执行
	if (HSIStartUpStatus == RCC_CR_HSIRDY) {
		// 调压器电压输出级别配置为 1，以便在器件为最大频率
		// 工作时使性能和功耗实现平衡
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;
		// HCLK = SYSCLK / 1
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		// PCLK2 = HCLK / 2
		RCC_PCLK2Config(RCC_HCLK_Div2);
		// PCLK1 = HCLK / 4
		RCC_PCLK1Config(RCC_HCLK_Div4);
		// 如果要超频就得在这里下手啦
		// 设置 PLL 来源时钟，设置 VCO 分频因子 m，设置 VCO 倍频因子 n，
		// 设置系统时钟分频因子 p，设置 OTG FS,SDIO,RNG 分频因子 q
		RCC_PLLConfig(RCC_PLLSource_HSI, m, n, p, q);
		// 使能 PLL
		RCC_PLLCmd(ENABLE);
		// 等待 PLL 稳定
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
		}
		/*-----------------------------------------------------*/
		// 配置 FLASH 预取指,指令缓存,数据缓存和等待状态
		FLASH->ACR = FLASH_ACR_PRFTEN
			| FLASH_ACR_ICEN
			| FLASH_ACR_DCEN
			| FLASH_ACR_LATENCY_5WS;
		/*-----------------------------------------------------*/
		// 当 PLL 稳定之后，把 PLL 时钟切换为系统时钟 SYSCLK
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		// 读取时钟切换状态位，确保 PLLCLK 被选为系统时钟
		while (RCC_GetSYSCLKSource() != 0x08) {
		}
	}
	else {
		// HSI 启动出错处理
		while (1) {
		}
	}
}
//------------------------------------------------------------------------------------------