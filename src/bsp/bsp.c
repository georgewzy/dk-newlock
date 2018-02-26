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

/*
*Function: rcc_config      
*Description: 系统时钟初始化  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void bsp_rcc_init(void)
{
	ErrorStatus err_status;

	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);

	err_status = RCC_WaitForHSEStartUp();

	if (err_status == SUCCESS)
	{        
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        
        /* PCLK1 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div1);
        
        /* PLLCLK = 8MHz * 4 = 32 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLDiv_2, RCC_PLLMul_4);
        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);
	
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08);
	}


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC , ENABLE);

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
}



/*
*Function: iwatchdog_config      
*Description: 独立看门狗初始化 
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void iwatchdog_config(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(0xFFE);
	IWDG_ReloadCounter();
	IWDG_Enable();		
}

/*
*Function: iwatchdog_clear      
*Description: 独立看门狗 清狗 
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void iwatchdog_clear(void)
{
	IWDG_ReloadCounter();
}

/*
*Function: gpio_config      
*Description: GPIO初始化
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;				                 //LED1
//  	gpio_init_structure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_Init(GPIOB, &gpio_init_structure);	

	
	// UART1
	gpio_init_structure.GPIO_Pin = GPIO_Pin_9;				// UART1 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;				
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	// UART2
	gpio_init_structure.GPIO_Pin = GPIO_Pin_2;				// UART2 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;				
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);


	// UART3
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;				// UART3 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			
  	GPIO_Init(GPIOB, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;				
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;			 
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
	

	
	//button
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOC, &gpio_init_structure);
	
	
	//LOCK ON   OFF
	gpio_init_structure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	//MOTO D+ MOTO D-
	gpio_init_structure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOC, &gpio_init_structure);
	
	//BELL
	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOC, &gpio_init_structure);
	
	
	//GPS power enable
	gpio_init_structure.GPIO_Pin = GPIO_Pin_4;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOC, &gpio_init_structure);
}



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
	
		// TIM3
	nvic_init_structure.NVIC_IRQChannel = TIM3_IRQn;
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;    //
    nvic_init_structure.NVIC_IRQChannelSubPriority = 3;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_structure);

	
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
*Function: bsp_init      
*Description: 底层初始化主函数  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void bsp_init(void)
{
	bsp_rcc_init();
	bsp_nvic_init();
	gpio_init();
//	pwm_gpio_init();
//	iwatchdog_config();
	usart1_init(115200, 8, 0, 1);
	usart2_init(115200);

	timer2_init(200, 41);
//	pwm_timer3_init(9999, 143);
//	pwm3_init(30);
 
	adc_init();
}





