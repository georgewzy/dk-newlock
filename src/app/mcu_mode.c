



#include "mcu_mode.h"






void mcu_mode_sleep(void)
{
	
}




void mcu_mode_low_power_run(void)
{

}

void mcu_mode_low_power_sleep(void)
{

}

void mcu_mode_stop_rtc(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RTC_WakeUpCmd(ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}


void mcu_mode_stop_without_rtc(void)
{
	
}


void mcu_mode_standby_rtc(void)
{

}



void mcu_mode_standby_without_rtc(void)
{

}






