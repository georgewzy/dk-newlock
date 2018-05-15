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

#ifndef __USART_H_
#define __USART_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  "bsp.h"
#include "stm32l1xx.h"
#include "stm32l1xx_usart.h"


#define USART_BUFF_LENGHT				   	1024

typedef struct
{
	uint16_t index;
	uint16_t len;
	uint8_t	 pdata[USART_BUFF_LENGHT];	
} usart_buff_t;







#define SerialBuffDefault() {0,{0}}




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void usart_gpio_init(void);
void usart1_init(uint32_t band_rate, uint8_t word_length, uint8_t parity, uint8_t stop_bit);
void usart2_init(uint32_t band_rate);
void usart4_init(uint32_t band_rate);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);


void usart1_recv_data(void);
void usart2_recv_data(void);


void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
void usart_send(USART_TypeDef* USARTx, uint8_t *data, uint16_t data_size,...);
void usart_send_data(USART_TypeDef* USARTx, uint8_t *data, uint16_t len, ...);
#endif




