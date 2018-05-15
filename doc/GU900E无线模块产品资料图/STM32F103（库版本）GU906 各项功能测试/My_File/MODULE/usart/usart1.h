#ifndef __USART1_H_
#define __USART1_H_
#include "sys.h"

#define USART1_BUFF     200  //接收数据缓存
#define USART1_485_EN   0    //1 开启485功能，0 关闭485功能
#define USART1_MAP_EN   0    //1 开启重定向功能，0关闭重定向功能

void usart1_Configuration(u32 bound);
void usart1_Send(char *buf,int len); 
int  usart1_Receive(char *buf, int buflen);
void usart1_FreeBuff(void);

#endif


