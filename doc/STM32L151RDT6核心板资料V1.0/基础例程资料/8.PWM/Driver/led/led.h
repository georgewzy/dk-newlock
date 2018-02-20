#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "general_type.h"
#include "stm32l1xx.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
void LED_Init(void);//³õÊ¼»¯
#define LED1_PORT 9
#define LED1_RUN(sts) (sts?( GPIOB->ODR |= (unsigned int)(1<<LED1_PORT) ):( GPIOB->ODR &= ~((unsigned int)(1<<LED1_PORT) )))

#define LED2_PORT 7
#define LED2_RUN(sts) (sts?( GPIOB->ODR |= (unsigned int)(1<<LED2_PORT) ):( GPIOB->ODR &= ~((unsigned int)(1<<LED2_PORT) )))

#define LED3_PORT 6
#define LED3_RUN(sts) (sts?( GPIOB->ODR |= (unsigned int)(1<<LED3_PORT) ):( GPIOB->ODR &= ~((unsigned int)(1<<LED3_PORT) )))

#define LED4_PORT 5
#define LED4_RUN(sts) (sts?( GPIOB->ODR |= (unsigned int)(1<<LED4_PORT) ):( GPIOB->ODR &= ~((unsigned int)(1<<LED4_PORT) )))


void led_on(u8 num);
#endif

















