

#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "usart.h"
#include "common.h"
#include "timer.h"
#include "transport.h"
#include "aes.h"
#include "motor.h"
#include "list.h"
#include "main.h"
#include "adc.h"
#include "flash.h"
#include "gprs.h"



extern uint8_t receiveText[24];
extern uint8_t expressText[128];  
extern uint8_t cipherText[128];
extern uint8_t aesKey[16];

extern uint8_t lock_open_err_flag;
extern uint8_t lock_close_err_flag;
extern uint8_t lock_status;
extern uint8_t lock_open_time_flag;
extern uint8_t lock_close_time_flag;

extern uint8_t lock_run_status;

//响铃
extern uint8_t lock_bell_flag;

//震动
extern uint8_t lock_shake_flag;

extern DEV_CONFIG_INFO  dev_config_info;



extern usart_buff_t mqtt_buff;
extern uint16_t mqtt_publish_msgid;

//电压
uint16_t Bat_V;
uint16_t Bat_Pre;


//保持在线状态
extern int keep_alive_status;
extern uint8_t mqtt_keep_alive_resend_flag;
extern uint8_t mqtt_keep_alive_err_cnt;


extern uint8_t gprs_status;	//GPRS状态


void protocol_analyze(void)
{
	uint8_t local_topic[50] = {0};
	uint8_t recv_topic[50] = {0};
	uint8_t payload[100];
	int payloadlen = 0;
	int mqtt_sub;
	
	mqtt_sub = mqtt_subscribe(recv_topic, payload, &payloadlen);
	
	if(mqtt_sub == 1)
	{
		USART_OUT(USART1, "AAAA=%s=%s\r\n", payload, recv_topic);
		
		sprintf((char*)local_topic, "%s%s", "lock/", dev_config_info.dev_id);
		if(strncmp((char*)recv_topic, (char*)"lock/", 5)==0)
		{	
			USART_OUT(USART1, "topic==%s\r\n", recv_topic);
					
			memset(receiveText , 0, 24);
			memset(expressText , 0, 128);
			
			strncpy((char*)receiveText, (char*)payload, payloadlen);
			AES_Decrypt(expressText, receiveText, aesKey);
			
//			USART_OUT(USART1, "receiveText=%s\r\n", receiveText);
//			USART_OUT(USART1, "expressText=%s\r\n", expressText);	
			if(*expressText == 0x31)
			{
//				timer_is_timeout_1ms(timer_open_lock, 0);
				lock_shake_flag = 1;
				lock_status = 1;
				lock_open_time_flag = 0;
				lock_run_status = 0;
				USART_OUT(USART1, "Lock_Open11111\r\n");
			
			}
			else if(*expressText == 0x32)
			{
//				timer_is_timeout_1ms(timer_close_lock, 0);
				lock_shake_flag = 1;
				lock_status = 0;
				lock_run_status = 0;
				lock_close_time_flag = 0;
				USART_OUT(USART1, "Lock_Close11111\r\n");
			}
			else if(*expressText == 0x30)
			{
				motor_stop();	//停止运行;
			}
								
			payloadlen = 0;
			memset(recv_topic, 0, 50);
			memset(payload, 0, 100);	
		}
		
		sprintf((char*)local_topic, "%s%s", "lock/", dev_config_info.dev_id);
		if(strncmp((char*)recv_topic,(char *)"bell/", 5)==0)
		{

			USART_OUT(USART1, "bell===========================================================================\r\n");

			lock_bell_flag = 1;
			
			payloadlen = 0;
			memset(recv_topic, 0, 50);
			memset(payload, 0, 100);
		}

	}
}



void protocol_analyze1(list_node **list)
{
	uint8_t local_topic[50] = {0};
	int list_status;
	mqtt_msg_s *msg = NULL;
	
	list_status = list_is_empty(list);	
	if(list_status == 1)	
	{	
		msg = list_get_addr_by_status(list, PUBCOMP);
		if(msg != NULL)
		{		
			if(msg->status == PUBCOMP)
			{		
				USART_OUT(USART1, "topic==%s\r\n", msg->topic);
				list_travese(list);
				sprintf((char*)local_topic, "%s%s", "lock/", dev_config_info.dev_id);
				if(strncmp((char*)msg->topic, (char*)"lock/", 5)==0)
				{					
					memset(receiveText , 0, 24);
					memset(expressText , 0, 128);
				
					strncpy((char*)receiveText, (char*)msg->payload, msg->payloadlen);
					AES_Decrypt(expressText, receiveText, aesKey);
					
	//				USART_OUT(USART1, "receiveText=%s\r\n", receiveText);
	//				USART_OUT(USART1, "expressText=%s\r\n", expressText);
	//				USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
					if(*expressText == 0x31)
					{
		//				timer_is_timeout_1ms(timer_open_lock, 0);
												
						lock_shake_flag = 1;
						lock_status = 1;
						lock_open_time_flag = 0;
						lock_run_status = 0;
						USART_OUT(USART1, "Lock_Open11111\r\n");
					}
					else if(*expressText == 0x32)
					{
		//				timer_is_timeout_1ms(timer_close_lock, 0);
						lock_shake_flag = 1;
						lock_status = 0;
						lock_run_status = 0;
						lock_close_time_flag = 0;
						USART_OUT(USART1, "Lock_Close11111\r\n");
					}
					else if(*expressText == 0x30)
					{
						motor_stop();	//停止运行;
					}				
				}
				
				sprintf((char*)local_topic, "%s%s", "bell/", dev_config_info.dev_id);
				if(strncmp((char*)msg->topic, (char*)"bell/", 5)==0)
				{
					USART_OUT(USART1, "bell===========================================================================\r\n");

					lock_bell_flag = 1;
				}
				
				list_de_by_msgid(list, msg->msg_id);
				list_travese(list);
			}
		}
	}
}







void dev_to_srv_batt_voltage(list_node **list, uint32_t ms)
{
	int mqtt_pub;
	uint8_t topic_buff[100] = {0};
	
	if(timer_is_timeout_1ms(timer_batt, ms) == 0)
	{
		Bat_V = adc_get_average(ADC_Channel_0, 10); 
		Bat_V = Bat_V*3300/4096;
		Bat_V = Bat_V*88/20;
		Bat_Pre = (Bat_V-5000)*100/2400;
		USART_OUT(USART1, "Bat_Pre=%s\r\n", Bat_Pre);
		
		memset(topic_buff, 0, 100);	
		memset(expressText, 0, 128);
		memset(cipherText, 0, 128);	
		
		sprintf((char *)topic_buff, "%s%s", "lockdata/", (char*)dev_config_info.dev_id);	
		sprintf((char *)expressText, "{%c%s%c:%s}",'"',"battery",'"',"20");
		AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
		
		USART_OUT(USART1, "dev_to_srv_batt_voltage\r\n");
		USART_OUT(USART1, "expressText=%s\r\n", expressText);
		USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
		USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
		
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 24, 2, mqtt_publish_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "dev_to_srv_batt mqtt_publist ok\r\n");
		}
		else
		{
			USART_OUT(USART1, "dev_to_srv_batt mqtt_publist error\r\n");
		}
	}
}




void dev_first_power_on(list_node **list)
{
	int mqtt_pub = 0;
	uint8_t heartbeat_buff[2] = {0};
	uint8_t topic_buff[100] = {0};
	
	memset(topic_buff, 0 ,100);
	memset(heartbeat_buff, 0, 2);
	
	sprintf((char*)topic_buff,"%s%s","lockdata/", dev_config_info.dev_id);
	heartbeat_buff[0] = 0x31;
	heartbeat_buff[1] = '\0';

	mqtt_pub = mqtt_publish_qos2(list, topic_buff, heartbeat_buff, 1, 2, mqtt_publish_msgid);
	if(mqtt_pub == 1)
	{
		USART_OUT(USART1, "dev_first_power_on ok\r\n");
	}	
	else
	{
		USART_OUT(USART1, "dev_first_power_on error\r\n");
	}	
}




void dev_to_srv_lock_status(list_node **list)
{
	int mqtt_pub = 0;
	uint8_t heartbeat_buff[2] = {0};
	uint8_t topic_buff[100] = {0};
	
	memset(topic_buff, 0, 100);
	memset(expressText, 0, 128);
	memset(cipherText, 0, 128);
	
	if(button_scan(LOCK_ON) == 0)	//开锁状态
	{	
		sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
		sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
		AES_Encrypt((char*)expressText, cipherText, aesKey);
		
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publish_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "dev_power_on_lock_status1 ok\r\n");
		}
		else
		{
			USART_OUT(USART1, "dev_power_on_lock_status1 error\r\n");
		}
	}
	else	//关锁状态
	{
		sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)dev_config_info.dev_id);
		sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
		AES_Encrypt((char*)expressText, (char*)cipherText, (char*)aesKey);
	
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 44, 2, mqtt_publish_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "dev_power_on_lock_status2 ok\r\n");
		}
		else
		{
			USART_OUT(USART1, "dev_power_on_lock_status2 error\r\n");
		}
	}
	
	
}


/*
void heartbeat(list_node **list, uint32_t ms)
{
	int mqtt_pub = 0;
	uint8_t heartbeat_buff[2] = {0};
	uint8_t topic_buff[100] = {0};
	
	if(timer_is_timeout_1ms(timer_heartbeat, ms) == 0)
	{
		memset(topic_buff, 0 ,100);
		memset(heartbeat_buff, 0, 2);
		
		sprintf((char*)topic_buff,"%s%s","lockdata/", dev_config_info.dev_id);
		heartbeat_buff[0] = 0x30;
		heartbeat_buff[1] = '\0';
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, heartbeat_buff, 1, 2, mqtt_publish_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "heartbeat_mqtt_publist_ok\r\n");
		}	
		else
		{
			USART_OUT(USART1, "heartbeat_mqtt_publist_error\r\n");
		}
	}
}
*/



void heartbeat_qos0(uint32_t ms)
{
	int mqtt_pub = 0;
	uint8_t heartbeat_buff[2] = {0};
	uint8_t topic_buff[100] = {0};
	
	if(timer_is_timeout_1ms(timer_heartbeat, ms) == 0)
	{
		memset(topic_buff, 0 ,100);
		memset(heartbeat_buff, 0, 2);
		
		sprintf((char*)topic_buff,"%s%s","lockdata/", dev_config_info.dev_id);
		heartbeat_buff[0] = 0x30;
		heartbeat_buff[1] = '\0';
		
		mqtt_pub = mqtt_publish_qos0(topic_buff, heartbeat_buff, 2);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "heartbeat_mqtt_publist_ok\r\n");
		}	
		else
		{
			USART_OUT(USART1, "heartbeat_mqtt_publist_error\r\n");
		}
	}
}




void mqtt_keep_alive(list_node *list_recv, list_node *list_send, int mqtt_stauts)
{	
	int gprs_wakeup_status = 0;
	//保持在线
	if(timer_is_timeout_1ms(timer_mqtt_keep_alive, MQTT_KEEP_ALIVE_INTERVAL) == 0 || mqtt_keep_alive_resend_flag == 1)
	{
		if(mqtt_keep_alive_resend_flag == 0)
		{
			mqtt_keep_alive_resend_flag = 1;
			gprs_wakeup_status = gprs_wakeup(0);
			if(gprs_wakeup_status == 1)
			{						
				USART_OUT(USART1, "mqtt_keep_alive_gprs_wakeup\r\n");
				keep_alive_status = mqtt_client(&list_recv, &list_send, PINGREQ);	
				if(keep_alive_status == PINGREQ)
				{
					timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);	
					USART_OUT(USART1, "mqtt_keep_alive_send_ok\r\n");
				}
			}
		}
			
		if((timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 1000*60) == 0))
		{
			mqtt_keep_alive_err_cnt++;
			if(mqtt_keep_alive_err_cnt > 5)
			{
				gprs_status = 254;
				mqtt_keep_alive_resend_flag = 0;
				mqtt_keep_alive_err_cnt = 0;
				USART_OUT(USART1, "system_reset\r\n");
				bsp_system_reset();	
			}
			else
			{
				gprs_wakeup_status = gprs_wakeup(0);
				if(gprs_wakeup_status == 1)
				{	
					USART_OUT(USART1, "mqtt_keep_alive_gprs_wakeup2\r\n");					
					keep_alive_status = mqtt_client(&list_recv, &list_send, PINGREQ);	
					if(keep_alive_status == PINGREQ)
					{		
						USART_OUT(USART1, "mqtt_keep_alive_resend_ok=%d\r\n", mqtt_keep_alive_err_cnt);
					}
				}	
			}
		}
	}
}







