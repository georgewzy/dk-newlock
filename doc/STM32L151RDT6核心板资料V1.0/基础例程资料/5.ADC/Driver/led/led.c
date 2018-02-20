#include "led.h"
#include "general_type.h"
void LED_Init(void)
{
    
  GPIO_InitTypeDef GPIO_InitStructure; 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_9);  
    	     	
}

void led_on(u8 unm)
{
    if(unm==1)
    {
        LED1_RUN(1);
    }
    else
    {
        LED1_RUN(0);
    }

    if(unm==2)
    {
        LED2_RUN(1);
    }
    else
    {
        LED2_RUN(0);
    }

    if(unm==3)
    {
        LED3_RUN(1);
    }
    else
    {
        LED3_RUN(0);
    }

    if(unm==4)
    {
        LED4_RUN(1);
    }
    else
    {
        LED4_RUN(0);
    }   
}
void Run_Led(void)
{
    static u8 i = 0 ;
    i = i ? 0:1;
    if(i)
    {
       LED1_RUN(1);
    }
    else
    {
        LED1_RUN(0);
    }
    
}




