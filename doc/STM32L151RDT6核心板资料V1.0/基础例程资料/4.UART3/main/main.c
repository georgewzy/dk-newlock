#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_usart.h"
#include "stm32l1xx_gpio.h"
#include "misc.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "stdio.h"

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len)
{ 
	uint16_t i;
	for(i=0; i<Len; i++){
		USART_SendData(USARTx, Data[i]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}
int fputc(int ch,FILE *f)
{
   USART_SendData(USART3,(u8)ch);
   while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//等待发送数据完毕);
   return ch;
}

void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	//GPIOB时钟
    
	  //NVIC_Configuration();
	
 	  USART_DeInit(USART3);  //复位串口1
    
	 //USART3_TX   PB.10  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    

	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    
    
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3,&USART_InitStructure);
		
    USART_Cmd(USART3,ENABLE);
		
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//开启USART2的接收中断
		//Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口2中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
		
		USART_ClearFlag(USART3,USART_FLAG_TC); //清除发送完成标志位 
		
}
int main(void)
{ 
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		delay_init(32);
	  USART_Configuration();
	  delay_ms(500);
    while(1)
    {
			//USART_OUT(USART3,"\r\nSTM3256789\r\n",12);
			printf("USART3 TEST\r\n");
			delay_ms(500);delay_ms(500);
		}
    
}



