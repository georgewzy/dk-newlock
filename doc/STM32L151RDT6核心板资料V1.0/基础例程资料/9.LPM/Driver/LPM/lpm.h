
#ifndef __LPM_H
#define __LPM_H 


#include "stm32l1xx.h"			   
#include "core_cm3.h"
#include "general_type.h"
typedef enum
{
	SLEEP = 0,   //睡眠模式
	Low_Power_Run = 1,
	Low_Power_Sleep = 2,
	Stop = 3,
	Standby = 4 //待机模式
}Low_Power_Mode;  

void PWR_VoltageScalingConfig(uint32_t PWR_VoltageScaling);
void Low_Power_Select(u8 LPM);
void Sleep_Mode(void);
void LP_Run_Mode(void);
void Stop_Mode(void);
void Standby_Mode(void);

#endif	//LPM.h end


/**************************FILE*END********************************/





























