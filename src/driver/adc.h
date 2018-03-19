

#ifndef __ADC_H_
#define __ADC_H_	
#include "stm32l1xx_adc.h"














void adc_gpio_init(void);
void adc_init(void);
uint16_t adc_get_value(uint8_t ch);
uint16_t adc_get_average(uint8_t ch, uint8_t times); 
 
#endif 
