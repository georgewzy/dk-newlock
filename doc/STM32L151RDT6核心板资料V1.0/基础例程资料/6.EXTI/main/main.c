#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_syscfg.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
u8 flag;
int main(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure PB.02 as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
	LED_Init();
	while(1);

}



