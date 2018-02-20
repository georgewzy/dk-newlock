#include "spi.h"

//void SPI1_Port_Init(void)
//{
//   
//    
//}
//void SPI1_Init(void)
//{	 		 
//	
//	//这里只针对SPI口初始化
//	SPI1_Port_Init();	


//    RCC->AHBENR |=  1<<0;    //使能PORTA时钟	
//    RCC->APB2ENR |= 1<<12;   	//SPI1时钟使能 
//    
//    RCC->APB2RSTR |= 1<<12;
//    RCC->APB2RSTR &= ~(1<<12);
//    
//    GPIOA->MODER   |= 2<<(5*2);//复用
//    GPIOA->OTYPER  |= 1<<5;//推挽输出
//    GPIOA->OSPEEDR |= 1<<(5*2);//2Mhz时钟
//    GPIOA->PUPDR   |= 1<<(5*2);//上拉
//    
//    GPIOA->MODER   |= 2<<(6*2);
//    GPIOA->OTYPER  |= 1<<6;
//    GPIOA->OSPEEDR |= 1<<(6*2);
//    GPIOA->PUPDR   |= 1<<(6*2);
//    
//    GPIOA->MODER   |= 2<<(7*2);
//    GPIOA->OTYPER  |= 1<<7;
//    GPIOA->OSPEEDR |= 1<<(7*2);
//    GPIOA->PUPDR   |= 1<<(7*2);
//    
//    GPIOA->AFR[1] &= ~((15<<4*7)|(15<<4*5)|(15<<4*6));
//    GPIOA->AFR[1] |= ((5<<4*7)|(5<<4*5)|(5<<4*6));//管脚映射到spi1
//   
//	SPI1->CR1 |= 0<<10;		//全双工模式	
//    
//	SPI1->CR1 |= 1<<9; 		//软件nss管理
//	SPI1->CR1 |= 1<<8;  

//	SPI1->CR1 |= 1<<2; 		//SPI主机
//	SPI1->CR1 |= 0<<11;		//8bit数据格式	
//	SPI1->CR1 |= 1<<1; 		//空闲模式下SCK为1 CPOL=1
//	SPI1->CR1 |= 1<<0; 		//数据采样从第二个时间边沿开始,CPHA=1  
//	//对SPI2属于APB1的外设.时钟频率最大为36M.
//	SPI1->CR1 |= 3<<3; 		//Fsck=Fpclk1/256
//	SPI1->CR1 |= 0<<7; 		//MSBfirst   
//	SPI1->CR1 |= 1<<6; 		//SPI设备使能
//	SPI1_ReadWrite(0xff);//启动传输		 
//}  
void SPI1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
 

//  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//上啦
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//时钟速率
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);  
    
 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//上啦
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//时钟速率
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);  



    
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);//GPIO_PinSource7
  /* SPI configuration -------------------------------------------------------*/
//  SPI_I2S_DeInit(SPI1);
   /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI设置为双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主spi
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE); //使能SPI外设
//  SPI1_SetSpeed(1);
//  SPI1_ReadWrite(0xff);//启动传输	
}
//SPI2速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB1/2^(SpeedSet+1)
//APB1时钟一般为36Mhz
void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;			//限制范围
	SPI1->CR1&=0XFFC7; 
	SPI1->CR1|=SpeedSet<<3;	//设置SPI2速度  
	SPI1->CR1|=1<<6; 		//SPI设备使能	  
} 
//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWrite(u8 TxData)
{		
	u16 retry=0;				 
	while((SPI1->SR&1<<1)==0)		//等待发送区空	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//超时退出
	}			  
	SPI1->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) 		//等待接收完一个byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//超时退出
	}	  						    
	return SPI1->DR;          		//返回收到的数据				    
}









