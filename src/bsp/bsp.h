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
#ifndef _BSP_H
#define _BSP_H
#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_usart.h>
#include <stm32l1xx_tim.h>
#include <stm32l1xx_iwdg.h>
#include "stm32l1xx_flash.h"
#include "misc.h"



//#define ENABLE						1
//#define DISABLE						0


#define SCL_HIGH()				GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define SCL_LOW()				GPIO_ResetBits(GPIOB, GPIO_Pin_0)

#define SDA_HIGH()				GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define SDA_LOW()				GPIO_ResetBits(GPIOB, GPIO_Pin_1)

#define SDA_READ()				GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)



#define LED0_HIGH()				GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define LED0_LOW()				GPIO_ResetBits(GPIOC, GPIO_Pin_0)



#define HAND_CLOSE_LOCK_READ()	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)		//按键
#define DS1A_READ()				GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)		//拨码开个
#define DS1B_READ()				GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)		//按键

#define LOCK_ON_READ()			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)
#define LOCK_OFF_READ()			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)



#define MOTOA_HIGH() 			GPIO_SetBits(GPIOB, GPIO_Pin_14)	
#define MOTOA_LOW() 			GPIO_ResetBits(GPIOB, GPIO_Pin_14)

#define MOTOB_HIGH() 			GPIO_SetBits(GPIOB, GPIO_Pin_15)	
#define MOTOB_LOW() 			GPIO_ResetBits(GPIOB, GPIO_Pin_15)


#define BEEP_ON() 				GPIO_SetBits(GPIOB, GPIO_Pin_11)	
#define BEEP_OFF() 				GPIO_ResetBits(GPIOB, GPIO_Pin_11)




//#define NVIC_VECTOR_ADDR			0x1C00

//#define NVIC_VECTOR_ADDR			0x0000


typedef enum
{
	HAND_CLOSE_LOCK,
	LOCK_ON,
	LOCK_OFF,
	DS1A,
	DS1B
	
} bsp_get_port;



uint8_t bsp_get_port_value(uint8_t port_name);
void bsp_rcc_clock_fre(void);
void iwatchdog_clear(void);
void bsp_system_reset(void);
void bsp_init(void);


#endif



