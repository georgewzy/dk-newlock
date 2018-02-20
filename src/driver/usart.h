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
#include <stm32l1xx.h>



#define USART_BUFF_LENGHT				   	512

typedef struct
{
	uint16_t index;	
	uint8_t	 pdata[USART_BUFF_LENGHT];	
} usart_buff_t;







#define SerialBuffDefault() {0,0,{0}}




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void usart1_init(uint32_t band_rate, uint8_t word_length, uint8_t parity, uint8_t stop_bit);
void usart2_init(uint32_t band_rate);
void usart3_init(uint32_t band_rate);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

void usart1_recv_data(void);
void usart2_recv_data(void);
void usart3_recv_data(void);

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
void usart_printf(USART_TypeDef* USARTx, uint16_t data_size, uint8_t *data,...);
#endif




