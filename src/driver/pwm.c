

#include "pwm.h"



void pwm_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	
	gpio_init_structure.GPIO_Pin = GPIO_Pin_0; //TIM2_CH3
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;     
	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_structure);

}


void pwm_timer3_init(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef tim_init_structure;

	tim_init_structure.TIM_Period = arr;               
	tim_init_structure.TIM_Prescaler =psc;             
	tim_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;            
	tim_init_structure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &tim_init_structure);   
	
	TIM_Cmd(TIM3, ENABLE); 
}

void pwm3_init(float dutyfactor)
{	
	TIM_OCInitTypeDef  tim_ocinit_structure;
		
	tim_ocinit_structure.TIM_OCMode = TIM_OCMode_PWM2;            
	tim_ocinit_structure.TIM_OutputState = TIM_OutputState_Enable;   
	tim_ocinit_structure.TIM_OCPolarity = TIM_OCPolarity_High; 	
	tim_ocinit_structure.TIM_Pulse = dutyfactor * 7200 /100;                                                           
	TIM_OC3Init(TIM3, &tim_ocinit_structure);    
	
	
	
	
}











