



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
DEV_CONFIG_INFO dev_config_info;





uint8_t *p1;
uint8_t *p2;



uint8_t protocol_buff[512] = {0};



uint8_t mqtt_keep_alive_flag = 0;
uint8_t mqtt_keep_alive_err_cnt = 0;


uint8_t lock_id[17] = {0};
uint8_t topic_buff[100] = {0};
//uint8_t send_buff[100] = {0};
uint8_t test_flag = 0;

int keep_alive_status = 0;










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
			list_clear(&list_send);	
			list_clear(&list_recv);				
			eeprom_read_data(EEPROM_LOCK_ID_ADDR, dev_config_info.dev_id, EEPROM_LOCK_ID_SIZE);
			eeprom_read_data(EEPROM_IP_ADDR, dev_config_info.dev_ip, EEPROM_IP_SIZE);
			eeprom_read_data(EEPROM_PORT_ADDR, dev_config_info.dev_port, EEPROM_PORT_SIZE);
			MakeFile_MD5_Checksum(dev_config_info.dev_id, EEPROM_LOCK_ID_SIZE);
			
			gprs_info.server_ip = dev_config_info.dev_ip;		
			gprs_info.server_port = atoi(dev_config_info.dev_port);

//			gprs_info.server_ip = "118.31.69.148";
//			gprs_info.server_port = 1883;
//			gprs_info.server_ip = "gpscore.net";
//			gprs_info.server_port = 4120;
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
			
			
//			memset(send_buff, 0, sizeof(send_buff));
//			sprintf(send_buff, "wangzhongya=%d", mqtt_publist_msgid);
			
			break;
		}
		else if(ds_val == 1)
		{
			test_dev();	
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
		
		gprs_init_task(&list_send, &gprs_info, &mqtt_data);

//		usart1_recv_data();		
		usart2_recv_data();	
		protocol_analyze1(&list_recv);
		
		
		mqtt_stauts = mqtt_client(&list_recv, &list_send, MQTTNULL);
		
		
		mqtt_keep_alive(list_recv, list_send, PINGREQ);	//保持在线
		

		lock_open_deal_1(&list_send);	//开锁处理
		lock_close_deal_1(&list_send);	//关锁处理	
		dev_to_srv_batt_voltage(&list_send, BATT_VOLTAGE);	//电池电压信息
		heartbeat1(HEARTBEAT);	//心跳
		
		
		lock_hand_close();	//手动关锁
		lock_self_checking();
		lock_find_bell();
		lock_shake_alarm();
		
		iwatchdog_clear();
		
//		if(timer_is_timeout_1ms(timer_system, 1000*2) == 0)
//		{
////			USART_OUT(USART1, "system running\r\n");
//			
//			rc = transport_sendPacketBuffer(0, "wangzhongya", 11);
//			if(rc != -1)
//			{
//				USART_OUT(USART1, "george\r\n");
//			}
//		}
		
//		if(timer_is_timeout_1ms(timer_system, 1000*20) == 0)
//		{
//			gprs_send_at("AT+CSQ\r\n", "OK", 30, 1000);
//		}
		
		
	}

}
 





















