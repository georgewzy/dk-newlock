



#include <stdarg.h>
#include <stdio.h>
#include <string.h>		
#include <stdlib.h>
#include "adc.h"
#include "bsp.h"
#include "timer.h"
#include "gprs.h"
#include "usart.h"
#include "aes.h"
#include "aes128.h"
#include "app_md5.h"
#include "transport.h"
#include "button.h"
#include "usart.h"
#include "flash.h"
#include "motor.h"
#include "main.h"
#include "lock.h"
#include "protocol.h"
#include "test.h"
#include "queue.h"
#include "list.h"

extern uint8_t lock_status1;

extern usart_buff_t mqtt_buff;
extern uint16_t mqtt_publist_msgid;
extern uint8_t gprs_status;
extern uint8_t usart2_buff[512];
extern uint16_t usart2_cnt; 

//响铃
extern uint8_t bell_flag;

//震动
extern uint8_t shake_flag;


extern uint8_t lock_open_err_flag;
extern uint8_t lock_close_err_flag;
extern uint8_t Lock_Open;
extern uint8_t Lock_Close;

extern uint8_t lock_status;
extern uint8_t lock_open_time_flag;
extern uint8_t lock_close_time_flag;

extern int lock_run_status;


extern sp_queue mqtt_recv_que;
extern sp_queue mqtt_send_que;



uint8_t receiveText[24];
uint8_t expressText[128];  
uint8_t cipherText[128];
uint8_t aesKey[16];

DEV_BASIC_INFO dev_basic_info;
DEV_CONFIG_INFO  dev_config_info;


//电压
uint16_t Bat_V;
uint16_t Bat_Pre;


uint8_t *p1;
uint8_t *p2;



uint8_t protocol_buff[512] = {0};



uint8_t mqtt_keep_alive_flag = 0;
uint8_t mqtt_keep_alive_err_cnt = 0;


uint8_t lock_id[17] = {0};
uint8_t topic_buff[100] = {0};
uint8_t send_buff[100] = {0};
uint8_t test_flag = 0;





void heartbeat(list_node **list, uint32_t ms)
{
	int mqtt_pub = 0;
	uint8_t heartbeat_buff[2] = {0};
	
	if(timer_is_timeout_1ms(timer_heartbeat, ms) == 0)
	{
		memset(topic_buff, 0 ,100);
		memset(heartbeat_buff, 0, 2);
		
		sprintf((char*)topic_buff,"%s%s","lockdata/", dev_config_info.dev_id);
		heartbeat_buff[0] = 0x30;
		heartbeat_buff[1] = '\0';
		
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, heartbeat_buff, 1, 2, mqtt_publist_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "heartbeat mqtt_publist ok\r\n");
		}	
		else
		{
			USART_OUT(USART1, "heartbeat mqtt_publist error\r\n");
		}
	}
}
int keep_alive_status = 0;

void mqtt_keep_alive1(list_node *list_recv, list_node *list_send, int mqtt_stauts)
{	
	int gprs_wakeup_status = 0;
	//保持在线
	if(timer_is_timeout_1ms(timer_mqtt_keep_alive, MQTT_KEEP_ALIVE_INTERVAL) == 0 || mqtt_keep_alive_flag == 1)
	{
		if(mqtt_keep_alive_flag == 0)
		{
			mqtt_keep_alive_flag = 1;
			gprs_wakeup_status = gprs_wakeup(0);
			if(gprs_wakeup_status == 1)
			{						
				keep_alive_status = mqtt_client(&list_recv, &list_send, PINGREQ);	
				if(keep_alive_status == PINGREQ)
				{
					timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);	
					USART_OUT(USART1, "mqtt_keep_alive send ok\r\n");
				}
			}
		}
			
		if((timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, MQTT_KEEP_ALIVE_INTERVAL/10) == 0))
		{
			mqtt_keep_alive_err_cnt++;
			if(mqtt_keep_alive_err_cnt > 6)
			{
				gprs_status = 0;
				mqtt_keep_alive_flag = 0;
				mqtt_keep_alive_err_cnt = 0;
				USART_OUT(USART1, "GPRS reset\r\n");
			}
			gprs_wakeup_status = gprs_wakeup(0);
			if(gprs_wakeup_status == 1)
			{						
				keep_alive_status = mqtt_client(&list_recv, &list_send, PINGREQ);	
				if(keep_alive_status == PINGREQ)
				{		
					USART_OUT(USART1, "mqtt_keep_alive resend ok\r\n");
				}
			}	
		}
	}
	
}


void dev_to_srv_batt_voltage(uint32_t ms)
{
	int mqtt_pub;
	
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
		
//		mqtt_pub = mqtt_publish(list, topic_buff, cipherText, 24, 2, mqtt_publist_msgid);
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

void dev_to_srv_batt_voltage1(list_node **list, uint32_t ms)
{
	int mqtt_pub;
	
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
		
		mqtt_pub = mqtt_publish_qos2(list, topic_buff, cipherText, 24, 2, mqtt_publist_msgid);
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





int main(void)
{ 
	int rc = 0;

	uint8_t ret = 0;
	uint8_t ds_val = 0;
	int mqtt_stauts = 0;
	GPRS_CONFIG gprs_info = GPRS_CONFIG_INIT;
	MQTTPacket_connectData mqtt_data = MQTTPacket_connectData_initializer;
	int gprs_wakeup_status = 0;

	list_node *list_recv = NULL;
	list_node *list_send = NULL;
	list_node *list_test1 = NULL;

	
	bsp_init();

	USART_OUT(USART1, "uart1 is ok\r\n");

	
	while(1)
	{	
		
//		list_test(&list_test1);
//		
//		list_test2(&list_test1);
		ds_val = button_ds_get_value();
		if(ds_val == 0)
		{
					
			eeprom_read_data(EEPROM_LOCK_ID_ADDR, dev_config_info.dev_id, EEPROM_LOCK_ID_SIZE);
			eeprom_read_data(EEPROM_IP_ADDR, dev_config_info.dev_ip, EEPROM_IP_SIZE);
			eeprom_read_data(EEPROM_PORT_ADDR, dev_config_info.dev_port, EEPROM_PORT_SIZE);
			MakeFile_MD5_Checksum(dev_config_info.dev_id, EEPROM_LOCK_ID_SIZE);
			
			gprs_info.server_ip = dev_config_info.dev_ip;		
			gprs_info.server_port = atoi(dev_config_info.dev_port);

//			gprs_info.server_ip = "emq.91daoke.com";
//			gprs_info.server_port = 1883;
//			gprs_info.server_ip = "103.46.128.47";
//			gprs_info.server_port = 14947;
			mqtt_data.clientID.cstring = dev_config_info.dev_id;
			mqtt_data.keepAliveInterval = MQTT_KEEP_ALIVE_INTERVAL;
			mqtt_data.cleansession = 1;
			mqtt_data.username.cstring = "daoke";
			mqtt_data.password.cstring = "DaokeEmq13245768";
			
			USART_OUT(USART1, "gprs_info.server_ip=%s\r\n", gprs_info.server_ip);	
			USART_OUT(USART1, "gprs_info.server_port=%d\r\n", gprs_info.server_port);
			USART_OUT(USART1, "mqtt_data.clientID.cstring=%s\r\n", mqtt_data.clientID.cstring);
			USART_OUT(USART1, "mqtt_data.username.cstring=%s\r\n", mqtt_data.username.cstring);
			USART_OUT(USART1, "mqtt_data.password.cstring=%s\r\n", mqtt_data.password.cstring);
			
			
			memset(send_buff, 0, sizeof(send_buff));
			sprintf(send_buff, "wangzhongya=%d", mqtt_publist_msgid);
			
			break;
		}
		else if(ds_val == 1)
		{
			
		}
		else if(ds_val == 2)	//test
		{	
			test_dev();	
		}
		else if(ds_val == 3)
		{		
			config_system();
		}
	}

	
	while(1)
	{
		
		gprs_init_task(&gprs_info, &mqtt_data);

//		usart1_recv_data();		
		usart2_recv_data();	
		protocol_analyze1(&list_recv);
		
		
		mqtt_stauts = mqtt_client(&list_recv, &list_send, MQTTNULL);
		if(mqtt_stauts == PINGRESP)
		{
			mqtt_keep_alive_flag = 0;
			mqtt_keep_alive_err_cnt = 0;
			USART_OUT(USART1, "mqtt_keep_alive recv ok\r\n");
		}		
		
		mqtt_keep_alive1(list_recv, list_send, PINGREQ);	//保持在线
		

		lock_open_deal_1(&list_send);	//开锁处理
		lock_close_deal_1(&list_send);	//关锁处理	
		dev_to_srv_batt_voltage1(&list_send, BATT_VOLTAGE);	//电池电压信息
		heartbeat(&list_send, HEARTBEAT);	//心跳
		
		
		lock_hand_close();	//手动关锁
		lock_self_checking();
		lock_find_bell();
		lock_shake_alarm();
		
		iwatchdog_clear();
		
//		if(timer_is_timeout_1ms(timer_system, 1000*10) == 0)
//		{
//			USART_OUT(USART1, "system running\r\n");
//		}
		
	}

}
 





















