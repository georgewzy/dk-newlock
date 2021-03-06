#ifndef __TIMER_H_
#define __TIMER_H_

#include "bsp.h"  






enum timer
{
    tim1_cnt,
	timer_test,
    timer_batt,
	timer_gps_cycle,
	tiemr_gps_location,
	timer_close_lock,
	timer_open_lock,
	timer_heartbeat,
	timer_lock,
	timer_bell_1,
	timer_bell_2,
	timer_uart1,
	timer_uart2,
	timer_uart3,
	timer_gprs,
	timer_at,
	timer_max
};




//如果index对应的时间超时，清零并返回1，未超时返回0
#define IS_TIMEOUT_1MS(index, count)    ((g_tim_cnt[(uint8_t)(index)] >= (count)) ?  	\
                                        ((g_tim_cnt[(uint8_t)(index)] = 0) == 0) : 0)



extern volatile uint32_t g_timer_cnt[(uint8_t)timer_max];

void timer2_init(uint16_t arr, uint16_t psc);
uint8_t timer_is_timeout_1ms(uint8_t type, uint32_t count);
void timer_delay_1ms(uint32_t ms);


 
#endif
