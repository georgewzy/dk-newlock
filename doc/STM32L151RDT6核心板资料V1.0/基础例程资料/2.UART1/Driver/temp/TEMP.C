#include "temp.h"
#include "stm32l1xx.h"
#include "adc.h" 
void get_temperature(u16 *temp)
{			  

	float temperate;		   
	temperate=Get_Adc_Average(ADC_CH_TEMP,1);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//计算出当前温度值
	*temp=temperate*10;//得到温度						   
//	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//最后添加结束符
}

