#ifndef __USART3_H_
#define __USART3_H_
#include "sys.h"

#define USART3_BUFF     200  //接收数据缓存
#define USART3_485_EN   0    //1 开启485功能，0 关闭485功能
#define USART3_MAP_EN   0    //1 开启重定向功能，0关闭重定向功能

void usart3_Configuration(u32 bound);
void usart3_Send(char *buf,int len); 
int  usart3_Receive(char *buf, int buflen);
void usart3_FreeBuff(void);

#endif


