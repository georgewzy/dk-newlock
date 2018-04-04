/*************************************************
  Copyright (C), 1988-1999,  Tech. Co., Ltd.
  File name:      
  Author:       Version:        Date:
  Description:   
  Others:         
  Function List:  
    1. ....
  History:         
                   
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "bsp.h"
#include "common.h"
#include "timer.h"
#include "usart.h"
#include "pwm.h"
#include "adc.h"
#include "button.h"
#include "motor.h"
#include "lock.h"
#include "gprs.h"





void bsp_hsi_init(void)
{
	RCC_DeInit();
	
	RCC_HSICmd(ENABLE);
	/* Check that HSI oscillator is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
}

/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_rcc_init(void)
{
	ErrorStatus err_status;

//	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	err_status = RCC_WaitForHSEStartUp();
	
	if (err_status == SUCCESS)
	{    
		/* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(ENABLE);
        
        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_1);
		
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        
		 /* PCLK1 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div1);
		
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        
        /* PLLCLK = 8MHz * 8 / 2 = 32 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLMul_8, RCC_PLLDiv_2);
		
        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);
	
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x0C);
	}

	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC , ENABLE);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		
}



/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_rcc_clock_fre(void)
{
	RCC_ClocksTypeDef get_rcc_clock;
	
	RCC_GetClocksFreq(&get_rcc_clock);
	
	USART_OUT(USART1, "SYSCLK=%d\r\n", get_rcc_clock.SYSCLK_Frequency);
}

/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void iwatchdog_config(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	IWDG_SetReload(0xFFE);
	IWDG_ReloadCounter();
	IWDG_Enable();		
}

/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void iwatchdog_clear(void)
{
	IWDG_ReloadCounter();
}


/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_system_reset(void) 
{ 
	__set_FAULTMASK(1); 
	NVIC_SystemReset();  
} 



/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;

//	gpio_init_structure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_12 | GPIO_Pin_15;
//  	gpio_init_structure.GPIO_Speed = GPIO_Speed_400KHz;
//	gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;          
//  	GPIO_Init(GPIOA, &gpio_init_structure);

//	gpio_init_structure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13;
//  	gpio_init_structure.GPIO_Speed = GPIO_Speed_400KHz;
//	gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;          
//  	GPIO_Init(GPIOB, &gpio_init_structure);
//	
//	gpio_init_structure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
//  	gpio_init_structure.GPIO_Speed = GPIO_Speed_400KHz;
//	gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;          
//  	GPIO_Init(GPIOC, &gpio_init_structure);
}




/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8_t bsp_get_port_value(uint8_t port_name)
{
	uint8_t value;
	
	switch(port_name)
	{
		case HAND_CLOSE_LOCK:
			value = HAND_CLOSE_LOCK_READ();
		break;
		
		case LOCK_ON:
			value = LOCK_ON_READ();
		break;
		
		case LOCK_OFF:
			value = LOCK_OFF_READ();
		break;
		
		case DS1A:
			value = DS1A_READ();
		break;
		
		case DS1B:
			value = DS1B_READ();
		break;
		
		
		default:
		break;
	}

	return value;
}



/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_nvic_init(void)
{
	NVIC_InitTypeDef nvic_init_structure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	// TIM2
	nvic_init_structure.NVIC_IRQChannel = TIM2_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;    //
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);
	
//		// TIM3
//	nvic_init_structure.NVIC_IRQChannel = TIM3_IRQn;
//    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;    //
//    nvic_init_structure.NVIC_IRQChannelSubPriority = 3;
//    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic_init_structure);

	
	// USART1
	nvic_init_structure.NVIC_IRQChannel = USART1_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 1;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;	 
	NVIC_Init(&nvic_init_structure);
	
	// USART2
	nvic_init_structure.NVIC_IRQChannel = USART2_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 2;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;	 
	NVIC_Init(&nvic_init_structure);	
	
}


/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void bsp_init(void)
{
	bsp_hsi_init();
	bsp_rcc_init();
	bsp_nvic_init();
	bsp_gpio_init();
	button_gpio_init();
	usart_gpio_init();
	lock_gpio_init();
	motor_gpio_init();
	gprs_gpio_init();
	adc_gpio_init();
//	pwm_gpio_init();
//	iwatchdog_config();
	usart1_init(115200, 8, 0, 1);
	usart2_init(115200);
	timer2_init(99, 319);
	adc_init();
//	pwm_timer3_init(9999, 143);
//	pwm3_init(30);
 
	
}





