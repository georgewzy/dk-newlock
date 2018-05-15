#ifndef __USART2_H_
#define __USART2_H_
#include "sys.h"

#define USART2_BUFF     200  //接收数据缓存
#define USART2_485_EN   0    //1 开启485功能，0 关闭485功能
#define USART2_MAP_EN   1    //1 开启重定向功能，0关闭重定向功能

void usart2_Configuration(u32 bound);
void usart2_Send(char *buf,int len); 
int  usart2_Receive(char *buf, int buflen);
void usart2_FreeBuff(void);

#endif


