#ifndef __ADC_H
#define __ADC_H	
//#include "sys.h"
#include "stm32l1xx_adc.h"
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void Adc_Init(void);
uint16_t  Get_Adc(uint8_t ch); 
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times); 
 
#endif 