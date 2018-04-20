




#include <stdarg.h>
#include <stdio.h>
#include <string.h>		
#include <stdlib.h>
#include "test.h"
#include "usart.h"
#include "timer.h"
#include "lock.h"
#include "gprs.h"



extern uint8_t protocol_buff[512];

extern uint8_t lock_self_run_status;
extern uint8_t lock_shake_flag;
extern uint8_t lock_open_time_flag;
extern uint8_t lock_close_time_flag;
extern uint8_t lock_run_status;
extern uint8_t lock_status;
extern uint8_t lock_bell_flag;
extern uint8_t lock_shake_flag;

extern void test_gprs(void);


void test_dev(void)
{
	uint8_t *p;
	uint8_t *ret;
	int tt =0;
	usart1_recv_data();
	
	p = strstr((uint8_t*)protocol_buff, "lock_self_checking=");
	if(p != NULL)
	{
		memcpy(&lock_self_run_status, (char*)(p+sizeof("lock_self_checking=")-1), 1);
		lock_self_run_status = atoi(&lock_self_run_status);
		lock_self_run_status = 0;
		lock_run_status = 255;
		lock_shake_flag = 1;
		USART_OUT(USART1, "lock_status=%s\r\n", &lock_self_run_status);
		memset(protocol_buff, 0, 512);
	}
	lock_self_checking();
	
	p = strstr((uint8_t*)protocol_buff, "lock_status=");
	if(p != NULL)
	{
		memcpy(&lock_status, (char*)(p+sizeof("lock_status=")-1), 1);
		lock_status = atoi(&lock_status);
		timer_is_timeout_1ms(timer_open_lock, 0);
		lock_shake_flag = 1;
		lock_run_status = 0;
		
		USART_OUT(USART1, "lock_status=%s\r\n", &lock_status);
		memset(protocol_buff, 0, 512);
	}
	test_lock_open();
	test_lock_close();
	
	
	
	p = strstr((uint8_t*)protocol_buff, "lock_bell=1");
	if(p != NULL)
	{
		lock_bell_flag = 1;
		USART_OUT(USART1, "test lock_bell_flag\r\n");
		memset(protocol_buff, 0, 512);
	}
	lock_find_bell();
	lock_shake_alarm();
	
	p = strstr((uint8_t*)protocol_buff, "CSQ=1");
	if(p != NULL)
	{
		ret = gprs_send_at("AT+CSQ\r\n", "OK", 500, 10000);
		if (ret != NULL)
		{
			
		}
		memset(protocol_buff, 0, 512);
	}
	
	p = strstr((uint8_t*)protocol_buff, "gprs_open=1");
	if(p != NULL)
	{
		test_gprs();
		memset(protocol_buff, 0, 512);
	}
}


void test_lock_open(void)
{
	if(lock_status == 1)
	{
		switch(lock_run_status)
		{
			case 0:
				if(button_scan(LOCK_ON) == 1)
				{	
					motor_forward();	//开锁				
					USART_OUT(USART1, "DDDlock_run_status0\r\n");
				}	
				timer_is_timeout_1ms(timer_open_lock, 0);
				lock_run_status = 1;
			break;
			
			case 1:
				if(button_scan(LOCK_ON) == 0)//正常开锁
				{
					lock_shake_flag = 0;	
					motor_stop();	//停止运行
					USART_OUT(USART1, "DDDlock_run_status1 A\r\n");
					lock_run_status = 2;	
				}
				
				if(timer_is_timeout_1ms(timer_open_lock, 5000) == 0)
				{	
					lock_run_status = 4;
					motor_reversal();
					USART_OUT(USART1, "DDDlock_run_status1 AA\r\n");
				}		
			break;
				
			case 2:
				lock_run_status = 3;
			break;
			
			case 3:
				if(button_scan(LOCK_ON) == 0) //正常开锁
				{
					lock_shake_flag = 0;	
					motor_stop();	//停止运行	
//					USART_OUT(USART1, "DDDlock_run_status2\r\n");
				}
			break;
			
			case 4:
				if(button_scan(LOCK_OFF) == 0) //开锁超时处理
				{
					motor_stop();	//停止运行
					lock_run_status = 5;
					USART_OUT(USART1, "DDD lock_run_status4\r\n");
				}		
			break;
			
			case 5:
				lock_run_status = 6;
			break;
			
			case 6:
				if(button_scan(LOCK_OFF) == 0) //开锁超时处理
				{
					motor_stop();	//停止运行
					lock_shake_flag = 0;
//					USART_OUT(USART1, "DDDlock_run_status4\r\n");
				}		
			break;
				
			case 255:
						
			break;
			
			default:
			break;
			
		}
	}
}



void test_lock_close(void)
{
	if(lock_status == 0)
	{
		switch(lock_run_status)
		{
			case 0:
				if(button_scan(LOCK_OFF) == 1)	//关锁
				{
					motor_reversal();
					USART_OUT(USART1, "EEE lock close0\r\n");
				}
				timer_is_timeout_1ms(timer_close_lock, 0);
				lock_run_status = 1;
			break;
			
			case 1:
				if(button_scan(LOCK_OFF) == 0)//正常关锁
				{
					motor_stop();	//停止运行
					lock_shake_flag = 0;	
					USART_OUT(USART1, "EEE lock close1 A\r\n");
					lock_run_status = 2;					
				}
				
				if(timer_is_timeout_1ms(timer_close_lock, 5000) == 0)
				{	
					lock_run_status = 4;
					motor_forward();
					USART_OUT(USART1, "EEE lock_run_status1 B\r\n");
				}
			break;


			case 2:
				lock_run_status = 3;
			break;
			
			case 3:
				if(button_scan(LOCK_OFF) == 0) //正常开锁
				{
					motor_stop();	//停止运行	
				}	
			break;
			
			case 4:
				if(button_scan(LOCK_ON) == 0) //关锁超时处理
				{
					lock_shake_flag = 0;
					motor_stop();	//停止运行									
					lock_run_status = 5;
				}		
				
			break;
				
			case 5:
				if(button_scan(LOCK_ON) == 0) //关锁超时处理
				{
					motor_stop();	//停止运行
					lock_shake_flag = 0;
				}
			break;
				
			case 255:
				
			break;
			
			
			default:
			break;
		}
	}

}


void test_gprs(void)
{
	int mqtt_rc = 0;

	uint8_t *ret;
	uint8_t buff[100] = {0};
	uint8_t cipstart[100] = {0};
	int test_gprs_status = 0;
	
	while(1)
	{
		switch(test_gprs_status)
		{
			case 0:
				gprs_power_on();
				
				USART_OUT(USART1, "gprs_power_on\r\n");
				test_gprs_status = 1;
	
				test_gprs_status++;
			break;
					
			case 1:
				ret = gprs_send_at("AT\r\n", "OK", 300,10000);
				if (ret != NULL)
				{
					test_gprs_status++;
				}
				
			break;
			
			case 2:
				ret = gprs_send_at("ATE0\r\n", "OK", 500,10000);
				if (ret != NULL)
				{
					test_gprs_status++;
				}
			
			break;
				
			case 3:
				ret = gprs_send_at("AT+CGSN\r\n", "OK", 500, 10000);
				if (ret != NULL)
				{
					test_gprs_status++;
				}
			break;
			
			case 4:			
//				ret = gprs_send_at("AT+CPIN?\r\n", "OK", 500, 10000);
//				if (ret != NULL)
				{
					test_gprs_status++;
				}
			
			break;
				
			case 5:
				ret = gprs_send_at("AT+CSQ\r\n", "OK", 500, 10000);
				if (ret != NULL)
				{
					test_gprs_status = 255;
				}
			break;
		
		
			case 255:			
				
			
			break;
		
		}	
		if(test_gprs_status == 255)
		{
			break;
		}
	}	
		
}


