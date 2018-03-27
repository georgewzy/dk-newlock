
#include "button.h"
#include "bsp.h"
#include "timer.h"


BUTTON_INFO button_info[BUTTON_SUM];


void button_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;	
	
	//HAND_CLOSE_LOCK
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6; 
	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN; 
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
	//DS
	gpio_init_structure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;	
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN; 
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOC, &gpio_init_structure);
	
	
	//LOCK ON OFF
	gpio_init_structure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_IN; 
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOB, &gpio_init_structure);
	
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



BUTTON_STATE button_get_state(uint8_t button_name, uint32_t long_time)
{
	
	uint8_t i = 0;
	uint8_t button_cnt = 0;
	uint8_t button_state = 0;
	
	
	for(i=0; i<BUTTON_VALID_CNT; i++)
	{
		if(bsp_get_port_value(button_name) == 0)
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
		if(button_info[button_name].butt_cnt < BUTTON_DOWN_CNT)
		{
			button_info[button_name].butt_cnt++;
		}
	}
	else
	{
		button_info[button_name].butt_en = DISABLE;
		button_info[button_name].butt_cnt = 0;
		button_info[button_name].butt_long_press_timer = long_time;
	}
	

	if(button_info[button_name].butt_cnt <= BUTTON_UP_CNT)  //有效次数小于等于弹起次数判断，则判断为弹起状态
	{
		button_info[button_name].butt_state = BUTTON_UP;
	}
	else if(button_info[button_name].butt_cnt <= BUTTON_DOWN_CNT)	//有效次数大于等于按下次数判断
	{
		if(BUTTON_UP == button_info[button_name].butt_state) //先前为弹起，则判断为单击
		{
			button_info[button_name].butt_state = BUTTON_CLICK;
		}
		else if(BUTTON_CLICK == button_info[button_name].butt_state)
		{
			button_info[button_name].butt_state = BUTTON_DOWN;
		}
		else if(BUTTON_DOWN == button_info[button_name].butt_state)
		{
			if(0 == button_info[button_name].butt_long_press_timer)
			{
				button_info[button_name].butt_state = BUTTON_PRESS;
			}
		}
	}
	
	button_state = button_info[button_name].butt_state;
	return button_state;
}




uint8_t button_scan(uint8_t button_name)
{
	
	uint8_t i = 0;
	uint8_t ret = 0;
	uint8_t button_cnt1 = 0;
	uint8_t button_cnt2 = 0;
	uint8_t button_state = 0;
	
	while(!ret)
	{
		if(bsp_get_port_value(button_name) == 0)
		{
			button_cnt2 = 0;
			button_cnt1++;
			if(button_cnt1 > 10)
			{
				button_state = 0;
				ret = 1;
			}
		}
		else
		{	
			button_cnt1 = 0;
			button_cnt2++;
			if(button_cnt2 > 10)
			{
				button_state = 1;
				ret = 1;
			}
		}
		timer_delay(10);
	}
	
	return button_state;
}




uint8_t button_ds_get_value(void)
{
	u8 val = 0;
	val = button_scan(DS1A); 
	val |= button_scan(DS1B)<<1;
		
	return val;
}


