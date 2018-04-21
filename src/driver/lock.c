

#include "lock.h"
#include "timer.h"
#include "motor.h"
#include "usart.h"
#include "button.h"
#include "transport.h"
#include "main.h"

extern uint16_t mqtt_publist_msgid;
extern uint8_t receiveText[24];
extern uint8_t expressText[128];  
extern uint8_t cipherText[128];
extern uint8_t aesKey[16];
//extern uint8_t lock_id[17];
extern uint8_t topic_buff[100];
extern uint8_t send_buff[100];
extern DEV_CONFIG_INFO  dev_config_info;


uint8_t lock_open_err_flag = 0;
uint8_t lock_close_err_flag = 0;


uint8_t lock_status = 255;		//初始化的时候不要初始化成0 1

uint8_t lock_open_time_flag = 0;
uint8_t lock_close_time_flag = 0;



uint8_t lock_shake_flag = 1 ;
uint8_t lock_bell_flag = 0;
	
uint8_t lock_run_status = 0;
uint8_t lock_self_run_status = 255;

uint8_t lock_status1 = 0;


void lock_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	//BELL
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOB, &gpio_init_structure);
}


 
void lock_shake_alarm(void)
{
	//晃动报警	
	if(1 == button_scan(LOCK_ON) && 1 == button_scan(LOCK_OFF) && lock_shake_flag == 0)
	{
		USART_OUT(USART1, "sharking\r\n");
		lock_bell_flag = 1;
	}
	
}


void lock_find_bell(void)
{
	if(lock_bell_flag == 1)
	{	
		lock_bell_flag = 0;
		BEEP_ON();
		timer_is_timeout_1ms(timer_bell, 0);
		USART_OUT(USART1, "BEEP_ON\r\n");
	}
	
	if(timer_is_timeout_1ms(timer_bell, 100) == 0)
	{
		BEEP_OFF();
	}
	
}

void lock_hand_close(void)
{
	if(BUTTON_CLICK == button_get_state(HAND_CLOSE_LOCK, 2000))
	{
		USART_OUT(USART1, "button_get_value\r\n");
		timer_is_timeout_1ms(timer_close_lock, 0);
		lock_shake_flag = 1;	
		lock_status = 0;
		lock_run_status = 0;
	}
}


void lock_self_checking(void)
{
	if(timer_is_timeout_1ms(timer_lock_self_checking, 1000*60*60*24) == 0)
	{
		lock_self_run_status = 0;
		lock_shake_flag = 1;
		timer_is_timeout_1ms(timer_open_lock, 0);
	}
	
	switch(lock_self_run_status)
	{
		case 0:
			if(button_scan(LOCK_ON) == 1)	//开锁
			{
				motor_forward();	//开锁	
				USART_OUT(USART1, "lock_run_status0\r\n");	
			}
			timer_is_timeout_1ms(timer_open_lock, 0);
			lock_self_run_status = 1;			
		break;
		
		case 1:
			if(button_scan(LOCK_ON) == 0)//正常开锁
			{
				lock_shake_flag = 0;	
				lock_self_run_status = 2;
				motor_reversal();	//关锁			
				USART_OUT(USART1, "lock_run_status1 A\r\n");
			}
			if(timer_is_timeout_1ms(timer_open_lock, 5000) == 0)
			{	
				lock_self_run_status = 3;
				motor_reversal();
				USART_OUT(USART1, "lock_run_status1 B\r\n");
			}
		break;
			
		case 2:
			if(button_scan(LOCK_OFF) == 0)//正常关锁
			{
				motor_stop();
				USART_OUT(USART1, "lock_run_status2\r\n");		
				lock_shake_flag = 0;
				lock_self_run_status = 255;
			}
		break;
						
		case 3:
			if(button_scan(LOCK_OFF) == 0)//开锁异常
			{
				motor_stop();
				USART_OUT(USART1, "lock_run_status3\r\n");
				lock_shake_flag = 0;
				lock_self_run_status = 255;
			}
		break;
			
		case 255:
			
		break;	
					
		default:
		break;		
	}
	
}


void lock_close_deal_1(list_node **list)
{
	int mqtt_pub = 0;

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
				USART_OUT(USART1, "EEE lock_stop\r\n");
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
				AES_Encrypt((char*)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				
				mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
				lock_run_status = 3;
			break;
			
			case 3:
				if(button_scan(LOCK_OFF) == 0) //正常开锁
				{
					lock_shake_flag = 0;	
					motor_stop();	//停止运行	
				}	
			break;
			
			case 4:
				if(button_scan(LOCK_ON) == 0) //关锁超时处理
				{
					motor_stop();	//停止运行
					USART_OUT(USART1, "EEE lock_run_status4\r\n");
					lock_run_status = 5;
				}			
			break;				
				
			case 5:
				USART_OUT(USART1, "EEE lock_run_status3\r\n");
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
				AES_Encrypt((char*)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				
				mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
				lock_run_status = 6;
			break;
				
			case 6:
				if(button_scan(LOCK_ON) == 0) //关锁超时处理
				{
					motor_stop();	//停止运行
					lock_shake_flag = 0;
				}
			break;
				
			case 7:
				
			break;
			
			
			default:
			break;
		}
	}
}

void lock_open_deal_1(list_node **list)
{
	int mqtt_pub = 0;
	
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
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
				AES_Encrypt((char*)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
			
				mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
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
				USART_OUT(USART1, "DDDlock_run_status3\r\n");
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
				AES_Encrypt((char*)expressText, (char*)cipherText, (char*)aesKey);
			
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
			
				mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
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
				
			case 7:
						
			break;
			
			default:
			break;
			
		}
	}
}




void lock_close_deal(void)
{
	int mqtt_pub;
	
	//关锁逻辑
	if(lock_status == 0)
	{
		if(timer_is_timeout_1ms(timer_close_lock, 5000) == 0 && lock_close_time_flag == 0)
		{
			motor_forward();
			lock_close_time_flag = 1;
			lock_close_err_flag = 1;
			USART_OUT(USART1, "Lock_Close timer\r\n");
			
		}
		if(button_scan(LOCK_OFF) == 1 && lock_close_time_flag == 0)
		{
			motor_reversal();
//			USART_OUT(USART1, "BBB lock close\r\n");
		}
		if(button_scan(LOCK_OFF) == 0)	//正常关锁
		{
			motor_stop();	//停止运行
			lock_shake_flag = 0;	
			
			if(lock_close_time_flag == 0)
			{
				lock_close_time_flag = 1;
				USART_OUT(USART1, "BBB lock_stop\r\n");
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
				AES_Encrypt((char*)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				
				mqtt_pub = mqtt_publish(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
			}
		}
		
		if(button_scan(LOCK_ON) == 0 && lock_close_err_flag == 1) //关锁超时处理
		{
			lock_close_err_flag = 0;
			motor_stop();	//停止运行
			lock_shake_flag = 0;
			USART_OUT(USART1, "BBB close lock unusual\r\n");
			
			memset(topic_buff, 0, 100);
			memset(expressText, 0, 128);
			memset(cipherText, 0, 128);
			
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
			sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
			AES_Encrypt((char*)expressText, (char*)cipherText, (char*)aesKey);
		
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
			
			mqtt_pub = mqtt_publish(topic_buff, expressText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
			else
			{
				USART_OUT(USART1, "mqtt_publist error\r\n");
			}
		}		
	}
}


void lock_open_deal(void)
{
	int mqtt_pub;
	//开锁逻辑		
	if(lock_status == 1)
	{
		if(timer_is_timeout_1ms(timer_open_lock, 5000) == 0 && lock_open_time_flag == 0)
		{
			lock_open_time_flag = 1;
			lock_open_err_flag = 1;
			motor_reversal();	
			USART_OUT(USART1, "Lock_Open timer\r\n");
		}
		if(button_scan(LOCK_ON)==1 && lock_open_time_flag == 0)
		{
			motor_forward();	//开锁	
			USART_OUT(USART1, "AAA lock_open\r\n");
		}
		if(button_scan(LOCK_ON) == 0) //正常开锁
		{
			lock_shake_flag = 0;	
			motor_stop();	//停止运行
			
			if(lock_open_time_flag == 0)
			{
				lock_open_time_flag = 1;
				USART_OUT(USART1, "AAA lock_stop\r\n");
				memset(topic_buff, 0, 100);
				memset(expressText, 0, 128);
				memset(cipherText, 0, 128);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
				sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
				AES_Encrypt((char*)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);

				mqtt_pub = mqtt_publish(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
				if(mqtt_pub == 1)
				{
					USART_OUT(USART1, "mqtt_publist ok\r\n");
				}
				else
				{
					USART_OUT(USART1, "mqtt_publist error\r\n");
				}
			}	
			
		}
		if(button_scan(LOCK_OFF) == 0 && lock_open_err_flag == 1)//开锁超时异常处理
		{
			lock_open_err_flag = 0;

			motor_stop();
			lock_shake_flag = 0;
			USART_OUT(USART1, "Lock_Open timerout\r\n");
			memset(topic_buff, 0, 100);
			memset(expressText, 0, 128);
			memset(cipherText, 0, 128);
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
			sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
			AES_Encrypt((char*)expressText, cipherText, aesKey);
			
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				
			mqtt_pub = mqtt_publish(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
			else
			{
				USART_OUT(USART1, "mqtt_publist error\r\n");
			}
		}
	}

}





