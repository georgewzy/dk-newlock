#ifndef __USART5_H_
#define __USART5_H_
#include "sys.h"

#define USART5_BUFF     200  //接收数据缓存
#define USART5_485_EN   1    //1 开启485功能，0 关闭485功能

void usart5_Configuration(u32 bound);
void usart5_Send(char *buf,int len); 
int  usart5_Receive(char *buf, int buflen);
void usart5_FreeBuff(void);

#endif



