/*
*******************************************************************************
*	                                  
*	模块名称 : printf模块    
*	文件名称 : printf.c
*	说    明 : 实现printf和scanf函数重定向到串口1，即支持printf信息到USART1
*				实现重定向，只需要添加2个函数:
*				int fputc(int ch, FILE *f);
*				int fgetc(FILE *f);
*				对于KEIL MDK编译器，编译选项中需要在MicorLib前面打钩，否则不会有数据打印到USART1。
*				
*				这个c模块无对应的h文件。
*
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>			


#if 1

struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

/*********************************************************
  * @function  fputc
  * @role      重定义putc函数，这样可以使用printf函数从串口1打印输出
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET){;}
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}

/*********************************************************
  * @function  fgetc
  * @role      重定义getc函数，这样可以使用scanff函数从串口1输入数据
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}


#endif

