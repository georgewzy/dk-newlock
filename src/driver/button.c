
#include "button.h"
#include "bsp.h"



button_info_s button_info[BUTTON_SUM];




void button_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	
	//button
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOC, &gpio_init_structure);
}


uint8_t button_get_value(void)
{
	uint8_t val = 0;
	uint8_t i = 0;
	uint8_t button_cnt = 0;
	
	for(i=0; i<30; i++)
	{
		if(BUTTON1_READ() == 0)
		{
			button_cnt++;
		}
		else
		{
			button_cnt = 0;
		}
	}
	
	if(button_cnt >= 30)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



void button_timer_ms(void)
{
	uint8_t i = 0;
	
	for(i=0; i<BUTTON_SUM; i++)
	{
		if(button_info[i].butt_en == ENABLE)
		{
			button_info[i].butt_long_press_timer--;
		}
	}
}



button_state_e button_get_state(uint8_t button_name, uint32_t long_time)
{
	
	uint8_t i = 0;
	uint8_t button_cnt = 0;
	uint8_t button_state = 0;
	
	
	for(i=0; i<BUTTON_VALID_CNT; i++)
	{
		if(BUTTON1_READ() == 0)
		{
			button_cnt++;
		}
		else
		{
			break;
		}
	}
	
	if(button_cnt >= BUTTON_VALID_CNT)
	{
		button_state = 0;
	}
	else
	{
		button_state = 1;
	}
		
	if(0 == button_state)
	{
		button_info[button_name].butt_en = ENABLE;
		if(button_info[button_name].butt_cnt < 5)
		{
			button_info[button_name].butt_cnt++;
		}
	}
	else
	{
		button_info[button_name].butt_cnt = 0;
		button_info[button_name].butt_long_press_timer = long_time;
	}
	
	if(button_info[button_name].butt_cnt <= 0)
	{
		button_info[button_name].butt_state = button_up;
	}
	else if(button_info[button_name].butt_cnt <= 5)
	{
		if(button_info[button_name].butt_state == button_up)
		{
			button_info[button_name].butt_state == button_click;
		}
		else if(button_click == button_info[button_name].butt_state)
		{
			if(0 == button_info[button_name].butt_long_press_timer)
			{
				button_info[button_name].butt_state == button_long_press;
			}
		
		}
	}
	
	
	return button_info[button_name].butt_state;
}










