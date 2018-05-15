//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                 写字楼里写字间，写字间里程序员；  
//                 程序人员写程序，又拿程序换酒钱。  
//                 酒醒只在网上坐，酒醉还来网下眠；  
//                 酒醉酒醒日复日，网上网下年复年。  
//                 但愿老死电脑间，不愿鞠躬老板前；  
//                 奔驰宝马贵者趣，公交自行程序员。  
//                 别人笑我忒疯癫，我笑自己命太贱；  
//                 不见满街漂亮妹，哪个归得程序员？  
//////////////////////////////////////////////////////////

/*
*******************************************************************************
*	                                  
*	模块名称 : usart模块    
*	文件名称 : usart3.c
*	说    明 : 实现串口3接口的数据接收与发送，支持串口重定向、232模式通信、
*              485模式通信，通过配置usart3.h文件来实现不同的通信方式，在
*			   485通信的情况下，控制IO得根据自己的实际情况做相应的改动。
*
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32_config.h"
//#include <string.h>
//#include "usart2.h"
//#include "delay.h"

/* 最大缓存 */
#define MAXRECV USART2_BUFF
static char RX_BUFF[MAXRECV]={0};  //接收缓冲
static int RX_SIZ=0;               //接收到的数据长度  

#if (USART2_485_EN == 1)
#define RS485_TX_EN	PAout(8)	//485模式控制.0,接收;1,发送.
#endif

/*********************************************************
  * @function  usart2_Configuration
  * @role      USART2串口初始化
  * @input     bound:波特率
  * @output    None
  * @return    None
  ********************************************************/
void usart2_Configuration(u32 bound)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#if (USART2_MAP == 0)
	/* config USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//PA2 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	
	//PA3 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);//开启端口B和复用功能时钟 
	GPIO_PinRemapConfig(GPIO_FullRemap_USART2,ENABLE);//使能端口重映射 
	//uart 的GPIO重映射管脚初始化 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//推挽输出 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//悬空输入 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
#endif
	
#if (USART2_485_EN == 1)
	//RE PA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		 //输入输出控制IO		
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
#endif

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 
	USART_ClearFlag(USART2, USART_FLAG_TC);       // 清标志
	RX_SIZ = 0;

#if (USART2_485_EN == 1)
	RS485_TX_EN=0;			//默认为接收模式
#endif
}

/*********************************************************
  * @function  USART2_IRQHandler
  * @role      串口接收中断
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void USART2_IRQHandler(void)
{
	char buff = 0;
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{	 	 
		if(RX_SIZ < MAXRECV){
			buff = USART_ReceiveData(USART2);//记录接收到的值
			if(buff || RX_SIZ){
				RX_BUFF[RX_SIZ++]  = buff;
			}
		} 
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);    //清除中断标志.
	}

	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)//溢出 
	{
		USART_ClearFlag(USART2,USART_FLAG_ORE);//读SR
		USART_ReceiveData(USART2);//读DR 
	}
}

/*********************************************************
  * @function  usart2_Send
  * @role      发送len个字节.
  * @input     要发送的数据与长度，
               (为了和本代码的接收匹配,这里建议不要超过 MAXRECV 个字节)
  * @output    None
  * @return    None
  ********************************************************/
void usart2_Send(char *buf,int len)
{
	int t;
#if (USART2_485_EN == 1)
	RS485_TX_EN=1;
#endif
  	for(t=0;t<len;t++)		//循环发送数据
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)==RESET){//发送一个字符完成产生中断
		USART_ClearITPendingBit(USART2 ,USART_IT_TXE);//清发送中断
	}
#if (USART2_485_EN == 1)
	RS485_TX_EN=0;
#endif
}

/*********************************************************
  * @function  usart2_Receive
  * @role      RS232查询接收到的数据
  * @input     buf:接收缓存首地址，与缓存长度
  * @output    None
  * @return    接收的数据长度
  ********************************************************/
int usart2_Receive(char *buf, int buflen)
{
	int rxlen=RX_SIZ;
	int i=0;
	int ret = 0;        //默认为0		
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RX_SIZ && rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;(i < rxlen) && (i < (buflen - 1));i++)
		{
			buf[i]=RX_BUFF[i];
			RX_BUFF[i] = 0;			
		}		
		buf[i]='\0';
		ret=i;	//记录本次数据长度
		RX_SIZ=0;		//清零
	}
	return ret;
}

/*********************************************************
  * @function  usart2_FreeBuff
  * @role      清空缓存中的数据
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void usart2_FreeBuff(void)
{
	int rxlen=RX_SIZ;
	int i=0;
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	while(rxlen)
	{
		if(rxlen==RX_SIZ && rxlen)//接收到了数据,且接收完成了
		{
			for(i=0;(i<rxlen) && (i < (MAXRECV-1)) ;i++){
				RX_BUFF[i] = 0;
			}	
			RX_SIZ=0;		//清零
		}
		delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
		rxlen=RX_SIZ;
	}
}



