

#include "timer.h"
#include "usart.h"
#include "button.h"



volatile uint32_t g_timer_cnt[(uint8_t)timer_max] = {0};
volatile uint32_t g_timeout_cnt = 0;
volatile uint32_t g_timer_send_list_pubrec_cnt[TIMER_SEND_LIST_PUBREC_MAX] = {0};
volatile uint32_t g_timer_send_list_pubrel_cnt[TIMER_SEND_LIST_PUBREL_MAX] = {0};


void timer2_init(uint16_t arr, uint16_t psc)
{

	TIM_TimeBaseInitTypeDef tim_init_structure;
	uint16_t cnt = 1000;

	TIM_DeInit(TIM2);  
    tim_init_structure.TIM_CounterMode = TIM_CounterMode_Up;
    tim_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init_structure.TIM_Period = arr;			//??????=(TIM_Prescaler+1)* (TIM_Period+1)/FLK
    tim_init_structure.TIM_Prescaler = psc; 	// 
    TIM_TimeBaseInit(TIM2, &tim_init_structure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_SetCounter(TIM2, 0);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}


void TIM2_IRQHandler(void)
{
	uint16_t i = 0, j = 0, k = 0;

	if (SET == TIM_GetITStatus(TIM2, TIM_IT_Update))
    {
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update);
		
		if (g_timeout_cnt != 0x00)	//???????
		{
			g_timeout_cnt--;
		}
			
        for (i = 0; i<(uint8_t)timer_max; i++)
        {
            g_timer_cnt[i]++;
        }	
		
		for (j=0; j<TIMER_SEND_LIST_PUBREC_MAX; j++)
		{
			g_timer_send_list_pubrec_cnt[j]++;
		}
		
		for (j=0; j<TIMER_SEND_LIST_PUBREL_MAX; j++)
		{
			g_timer_send_list_pubrel_cnt[j]++;
		}
		
		button_timer_ms();		//???????
    }
}


void timer_delay(uint32_t count)
{ 
	while(count--);					//
}




/*
*********************************************************************************************************
*                                          timer_delay_1ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void timer_delay_1ms(uint32_t ms)
{
	g_timeout_cnt = ms;		 
	while(g_timeout_cnt != 0);					//
}



/*
*********************************************************************************************************
*                                          timer_is_timeout_1MS()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8_t timer_is_timeout_1ms(uint8_t type, uint32_t count)
{
	int status = 0;
	
	if (g_timer_cnt[(uint8_t)(type)] >= count)
	{
		g_timer_cnt[(uint8_t)(type)] = 0;
		status = 0;
	}
	else
	{
		status = 1;
	}
	
	return status;
}


uint8_t timer_send_list_pubrec_1ms(uint8_t type, uint32_t count)
{
	int status = 0;
	
	if (g_timer_send_list_pubrec_cnt[type] >= count)
	{
		g_timer_send_list_pubrec_cnt[type] = 0;
		status = 0;
	}
	else
	{
		status = 1;
	}
	
	return status;	
}


uint8_t timer_send_list_pubrel_1ms(uint8_t type, uint32_t count)
{
	int status = 0;
	
	if (g_timer_send_list_pubrel_cnt[type] >= count)
	{
		g_timer_send_list_pubrel_cnt[type] = 0;
		status = 0;
	}
	else
	{
		status = 1;
	}
	
	return status;	
}

















