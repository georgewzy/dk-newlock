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

	RCC_DeInit();
	
	RCC_HSICmd(ENABLE);
	/* Check that HSI oscillator is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	
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
        
        /* PLLCLK = 8MHz * 4 = 32 MHz */
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
	IWDG_SetPrescaler(IWDG_Prescaler_256);
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
	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;				                 //LED1
//  	gpio_init_structure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_Init(GPIOB, &gpio_init_structure);	

	
	// UART1
	gpio_init_structure.GPIO_Pin = GPIO_Pin_9;				// UART1 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;	
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;				
	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	// UART2
	gpio_init_structure.GPIO_Pin = GPIO_Pin_2;				// UART2 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;				
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	//SW1
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;  
	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;				
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
	//DS
	gpio_init_structure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;	
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN; 
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOC, &gpio_init_structure);
	
	
	//LOCK ON OFF
	gpio_init_structure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;          
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
	//MOTO D+ MOTO D-
	gpio_init_structure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
	//BELL
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOB, &gpio_init_structure);
		
	//GPRS POWER EN
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	//GPRS PWON
	gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	//GPRS DTR
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
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
	bsp_rcc_init();
	bsp_nvic_init();
	bsp_gpio_init();
//	button_gpio_init();
	adc_gpio_init();
//	pwm_gpio_init();
//	iwatchdog_config();
	usart1_init(115200, 8, 0, 1);
	usart2_init(115200);

	timer2_init(99, 319);
//	pwm_timer3_init(9999, 143);
//	pwm3_init(30);
 
	adc_init();
}





