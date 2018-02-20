#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_syscfg.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "lpm.h"
int main(void)
{ 
	delay_ms(5000);
	Low_Power_Select(SLEEP);
	while(1);

}



