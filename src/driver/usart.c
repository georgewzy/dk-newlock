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
*************************************************/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "timer.h"
#include "common.h"


extern uint8_t protocol_buff[512];
extern usart_buff_t mqtt_buff;
extern int mqtt_buff_cnt;




usart_buff_t sb = SerialBuffDefault();
usart_buff_t usart1_rx_buff = SerialBuffDefault();
usart_buff_t usart2_rx_buff = SerialBuffDefault();
usart_buff_t usart3_rx_buff = SerialBuffDefault();
usart_buff_t mqtt_buff = SerialBuffDefault();
usart_buff_t at_rx_buff = SerialBuffDefault();


//u8 usart1_buff[USART_BUFF_LENGHT] = {0};
//u8 usart2_buff[USART_BUFF_LENGHT] = {0};
//u8 usart3_buff[USART_BUFF_LENGHT] = {0};
//u16 usart1_cnt = 0;
//u16 usart2_cnt = 0;
//u16 usart3_cnt = 0;

uint8_t usart1_rx_status = 0;
uint8_t usart2_rx_status = 0;
uint8_t usart3_rx_status = 0;
uint8_t usart4_rx_status = 0;




#pragma import(__use_no_semihosting)               
//??????????                   
struct __FILE  
{  
    int handle;  
  
};  
FILE __stdout;  
  
  
//???fputc??,???????????????  
int fputc(int ch, FILE *f)  
{  
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
    USART_SendData(USART1, (uint8_t) ch);  
    return ch;  
}  



void usart_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	
	// UART1
	gpio_init_structure.GPIO_Pin = GPIO_Pin_9;				// UART1 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;	
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_10;	
	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	// UART2
	gpio_init_structure.GPIO_Pin = GPIO_Pin_2;				// UART2 TX				    
  	gpio_init_structure.GPIO_Mode = GPIO_Mode_AF;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;			
  	GPIO_Init(GPIOA, &gpio_init_structure);
	gpio_init_structure.GPIO_Pin = GPIO_Pin_3;				
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;			 
  	GPIO_Init(GPIOA, &gpio_init_structure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

}



/*
*********************************************************************************************************
*                                          usart1_init()
*
* Description : 对usart1进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart1_init(uint32_t band_rate, uint8_t word_length, uint8_t parity, uint8_t stop_bit)
{
	
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = word_length;
	usart_init_structre.USART_StopBits = stop_bit;
	usart_init_structre.USART_Parity = parity;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart_init_structre);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	
}





/*
*********************************************************************************************************
*                                          usart2_init()
*
* Description : 对usart2进行初始化.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart2_init(uint32_t band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &usart_init_structre);
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
}




/*
*********************************************************************************************************
*                                          usart3_init()
*
* Description : 对usart3进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart4_init(uint32_t band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &usart_init_structre);
		
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(UART4, ENABLE);
	
}



/*
*********************************************************************************************************
*                                          USART1_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART1_IRQHandler(void)
{
	uint8_t ch = 0;	
	
   	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
				
        ch = USART_ReceiveData(USART1);	 
		
		timer_is_timeout_1ms(timer_uart1, 0);
		
//		if(usart1_rx_status == 0)
		{
			if (usart1_rx_buff.index < USART_BUFF_LENGHT)
			{	
				usart1_rx_buff.pdata[usart1_rx_buff.index++] = ch;
			}
			else
			{
				memset(&usart1_rx_buff, 0, sizeof(usart1_rx_buff));	//清理缓冲区		
			}
		}
	}
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					   
  	}		
}


void usart1_recv_data(void)
{
	
	if(timer_is_timeout_1ms(timer_uart1, 20) == 0)	//40ms没接收到数据认为接收数据完成		
	{
		usart_send_data(USART1, usart1_rx_buff.pdata, usart1_rx_buff.index);
		
		memcpy(protocol_buff, usart1_rx_buff.pdata, 512);
		memset(&usart1_rx_buff, 0, sizeof(usart1_rx_buff));
	}

}

/*
*********************************************************************************************************
*                                          USART2_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART2_IRQHandler(void)
{
	uint8_t ch = 0;	

   	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
		
		timer_is_timeout_1ms(timer_uart2, 0);		//定时器清零		
//		if(usart2_rx_status == 0)
		{
			ch = USART_ReceiveData(USART2);	 
//			USART_SendData(USART1, ch);
			if (usart2_rx_buff.index < USART_BUFF_LENGHT)
			{			
				usart2_rx_buff.pdata[usart2_rx_buff.index++] = ch;
			}
			else
			{
				memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	//清理缓冲区
			}
		}
	}
	
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					   
  	}	
}

/*
*********************************************************************************************************
*                                          usart2_recv_data()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/  
void usart2_recv_data(void)
{		
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL; 
	uint8_t pick_str[50] = {0};
	int data_len = 0,i =0;
	
	if(timer_is_timeout_1ms(timer_uart2, 10) == 0)	//20ms没接收到数据认为接收数据完成		
	{
		if(usart2_rx_buff.index > 0)
		{
			p1 = strstr((const char*)usart2_rx_buff.pdata, "+IPD");
			if(p1 != NULL)
			{
				p2 = str_picked(p1, ",", ":", (char*)pick_str);	//取出数据长度
				if(p2 != NULL)
				{
					data_len = atoi((char*)pick_str);
				}
				p3 = strstr((const char*)usart2_rx_buff.pdata, ":");

				if(mqtt_buff.index+data_len < USART_BUFF_LENGHT)
				{
					memcpy(&mqtt_buff.pdata[mqtt_buff.index], p3+1, data_len);	//copy数据
					mqtt_buff.index += data_len;
					mqtt_buff.len += data_len;
				}
				USART_OUT(USART1, "AA:");
				usart_send_data(USART1, usart2_rx_buff.pdata, usart2_rx_buff.index);
			}
			else
			{
				memset(&at_rx_buff, 0, sizeof(at_rx_buff));
				memcpy(&at_rx_buff.pdata, usart2_rx_buff.pdata, usart2_rx_buff.index);	
				at_rx_buff.index = usart2_rx_buff.index;
				USART_OUT(USART1, "CC:");
				usart_send_data(USART1, at_rx_buff.pdata, at_rx_buff.index);
			}	
		}
		memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	//清理缓冲区
	}	
}


/*
*********************************************************************************************************
*                                          USART_OUT()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : 但是在仔细看手册的时候发现 TC 和 TXE 标志位在复位的时候被置1 ，这样第一次while循环就是没有用的。这样导致了首次第一个字符还没有被输出，
*               就被后面的字符覆盖掉，造成实际看到的丢失现象。解决办法就很简单：在前面加上一句 USART1->SR
*********************************************************************************************************
*/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
{ 
	const char *s;
    int d;  
    char buf[32];
    
	va_list ap;
    __va_start(ap, Data);
	
	
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);	//
	while(*Data != '\0')
	{				                         
		if(*Data==0x5c)
		{									
			switch (*++Data)
			{
				case 'r':							          
					USART_SendData(USARTx, 0x0d);	   
					Data++;
				break;
				case 'n':							          
					USART_SendData(USARTx, 0x0a);	
					Data++;
				break;
				
				default:
					Data++;
			    break;
			}						 
		}
		else if(*Data=='%')
		{									  //
			switch (*++Data){				
				case 's':										  
                	s = __va_arg(ap, const char *);
                	for ( ; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                break;
            	case 'd':										 
                	d = __va_arg(ap, int);
					
					sprintf(buf, "%d", d);
                	for (s = buf; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                break;
				case 'x':										 
                	s = __va_arg(ap, const char *);
				
					int n = atoi(s);
					itoa(n, buf, 16);
//					strtol(buf, &s, 16);
//					sprintf(buf, "%x", s);
                	for (s = buf; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                break;	
				default:
					Data++;
				    break;
			}		 
		}
		else 
			USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}



//此函数有bug
void usart_send(USART_TypeDef* USARTx, uint8_t *data, uint16_t data_size,...)
{ 
	const char *s;
    int d;  
    char buf[32];
    
	va_list ap;
    __va_start(ap, data);
	
	
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);	//
	while(data_size--)
	{				                         
		if(*data==0x5c)
		{									
			switch (*++data)
			{
				case 'r':							          
					USART_SendData(USARTx, 0x0d);	   
					data++;
				break;
				case 'n':							          
					USART_SendData(USARTx, 0x0a);	
					data++;
				break;
				
				default:
					data++;
			    break;
			}						 
		}
		else if(*data=='%')
		{									  //
			switch (*++data){				
				case 's':										  
                	s = __va_arg(ap, const char *);
                	for ( ; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					data++;
                	break;
            	case 'd':										 
                	d = __va_arg(ap, int);

					sprintf(buf, "%d", d);
                	for (s = buf; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					data++;
                	break;
				default:
					data++;
				   break;
			}		 
		}
		else 
			USART_SendData(USARTx, *data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
	
}

void usart_send_data(USART_TypeDef* USARTx, uint8_t *data, uint16_t len, ...)
{ 
//	USART_ReceiveData(USARTx);
	
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);
	
	while( len!=0 )
	{				                         
		USART_SendData(USARTx, *data++);
		len--;
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);	
	}
}












