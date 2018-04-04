




#include <stdarg.h>
#include <stdio.h>
#include <string.h>		
#include <stdlib.h>
#include "test.h"
#include "usart.h"
#include "timer.h"
#include "lock.h"




extern uint8_t protocol_buff[512];
extern uint8_t lock_self_status;
extern uint8_t lock_run_status;
extern uint8_t shake_flag;
extern uint8_t lock_open_time_flag;
extern uint8_t lock_close_time_flag;



void test_dev(void)
{
	uint8_t *p;
	int tt =0;
	usart1_recv_data();

	
	p = strstr((uint8_t*)protocol_buff, "self_checking=");
	if(p != NULL)
	{
		memcpy(&lock_self_status, (char*)(p+sizeof("self_checking=")-1), 1);
		lock_self_status = atoi(&lock_self_status);
		lock_run_status = 0;
		timer_is_timeout_1ms(timer_open_lock, 0);
		USART_OUT(USART1, "lock_status=%s\r\n", &lock_self_status);
		memset(protocol_buff, 0, 512);
	}
	lock_self_checking();
	p = strstr((uint8_t*)protocol_buff, "open_lock=");
	if(p != NULL)
	{
		memcpy(&lock_self_status, (char*)(p+sizeof("open_lock=")-1), 1);
		lock_self_status = atoi(&lock_self_status);
		timer_is_timeout_1ms(timer_open_lock, 0);

		shake_flag = 1;
		lock_open_time_flag = 0;
		lock_close_time_flag = 0;
		USART_OUT(USART1, "lock_status=%s\r\n", &lock_self_status);
		memset(protocol_buff, 0, 512);
	}

	lock_close_deal();
	lock_open_deal();
	
}



