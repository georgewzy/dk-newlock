


#include "motor.h"




void motor_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	
	//MOTO + MOTO -
	gpio_init_structure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;          
  	GPIO_Init(GPIOB, &gpio_init_structure);
}




void lock_open(void)
{
	MOTOA_HIGH();
	MOTOB_LOW();
}

void lock_close(void)
{
	MOTOA_LOW();
	MOTOB_HIGH();
}

void lock_stop(void)
{
	MOTOA_LOW();
	MOTOB_LOW();
}








uint8_t lock_on_status_get(void)
{
	uint8_t i = 0;
	uint8_t ret = 0;
	uint8_t cnt = 0;
	for(i=0; i<30; i++)
	{
		if(LOCK_ON_READ() == 0)
		{
			cnt++;
		}
		else
		{
			cnt = 0;
		}
	}
	
	if(cnt >= 10)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	
	return ret;
}


uint8_t lock_off_status_get(void)
{
	uint8_t i = 0;
	uint8_t ret = 0;
	uint8_t cnt = 0;
	
	for(i=0; i<30; i++)
	{
		if(LOCK_OFF_READ() == 0)
		{
			cnt++;
		}
		else
		{
			cnt = 0;
		}
	}
	if(cnt >= 10)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	
	return ret;
}













