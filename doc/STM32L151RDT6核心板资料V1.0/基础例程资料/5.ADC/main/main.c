#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_usart.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_ADC.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
u8 flag;
char vol[6] = {"0.000v"};
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len)
{ 
	uint16_t i;
	for(i=0; i<Len; i++){
		USART_SendData(USARTx, Data[i]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_96Cycles );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 
int main(void)
{ 
    u16 adc_value;
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
		delay_init(32);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	//GPIOA时钟
    
   	USART_DeInit(USART1);  //复位串口1
    
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    
    
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1,&USART_InitStructure);
    USART_Cmd(USART1,ENABLE);
    
    /*----------------- ADC1 configuration with DMA enabled --------------------*/
    /* Enable the HSI oscillator */
    RCC_HSICmd(ENABLE);

    /* Enable GPIOB clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    /* Configure PB.12 (ADC Channel8) in analog mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Check that HSI oscillator is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* ADC1 configuration */
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel18 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_4Cycles);   
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Wait until the ADC1 is ready */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
    {
    }    
    ADC_SoftwareStartConv(ADC1);      
    while(1)
    {
        adc_value = Get_Adc_Average(8,10);  
				adc_value = (adc_value*3.3/4095)*1000;
				vol[0] = (adc_value/1000)+0x30;
			  vol[1] = '.';
				vol[2] = ((adc_value%1000)/100)+0x30;
				vol[3] = ((adc_value%100)/10)+0x30;
				vol[4] = (adc_value%10)+0x30;
				USART_OUT(USART1,(uint8_t*)vol,(uint16_t)strlen((char*)vol));
			  USART_OUT(USART1,"\r\n",2);
				delay_ms(1000);
    }
}



