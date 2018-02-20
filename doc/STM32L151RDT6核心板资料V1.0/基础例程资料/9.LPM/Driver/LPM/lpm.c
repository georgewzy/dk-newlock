#include "lpm.h"

#define CR_VOS_MASK              ((uint32_t)0xFFFFE7FF)
#define PWR_VoltageScaling_Range1       PWR_CR_VOS_0
#define PWR_VoltageScaling_Range2       PWR_CR_VOS_1
#define PWR_VoltageScaling_Range3       PWR_CR_VOS


/*******************************************************************
函 数 名：__asm void WFI_SET(void)
功能说明：THUMB指令不支持汇编内联
		  采用如下方法实现执行汇编指令WFI
参	  数：无
返 回 值：无
*******************************************************************/


void Low_Power_Select(u8 LPM)
{
	switch(LPM)
		{
			case 0://睡眠模式
				Sleep_Mode();
				break;
			case 1://低电压运行
				LP_Run_Mode();
				break;
			case 2://低电压睡眠模式
				
				break;
			case 3://停止模式
				Stop_Mode();
				break;
			case 4://待机模式
				Standby_Mode();
				break;
		}
}
void Sleep_Mode(void)
{
	RCC->APB1ENR|=1<<28;     //使能电源时钟	
	PWR->CR &= ~(3<<0);		//清除PDDS 和LPDSR
	PWR->CR |= 1<<0;		//set LPDSR
	/* Clear SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
	__WFI();				 //执行WFI指令	
}
/* PWR_EnterLowPowerRunMode */
void LP_Run_Mode(void)
{
	RCC->APB1ENR |= 1<<28;     //使能电源时钟
	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);//1.5v
	PWR->CR |= PWR_CR_LPSDSR;
    PWR->CR |= PWR_CR_LPRUN;   	
}
void Stop_Mode(void)
{

	/*进入low power mode*/
	LP_Run_Mode();
	RCC->APB1ENR|=1<<28;     //使能电源时钟	

	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);//1.5v
	 
	PWR->CR &= ~(3<<0);		//清除PDDS 和LPDSR
	PWR->CR |= 1<<0;		//set LPDSR
	/* Set SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR |= SCB_SCR_SLEEPDEEP;
	RCC->APB1ENR &= ~(1<<28);     /*关闭电源时钟*/	
	__WFI();
	/* Reset SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);  	
}

void Standby_Mode(void)
{
	RCC->APB1ENR|=1<<28;     //使能电源时钟	 
	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);//1.5v	
	/* Clear Wakeup flag */
	PWR->CR |= PWR_CR_CWUF; 
	/* Select STANDBY mode */
	PWR->CR |= PWR_CR_PDDS; 
	/* Set SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR |= SCB_SCR_SLEEPDEEP;
	PWR->CSR |= 1<<8;//PA0 唤醒中断
	/* Request Wait For Interrupt */
	__WFI();
	/* Reset SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP); 
}






