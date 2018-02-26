

#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "usart.h"
#include "common.h"



extern uint8_t usart2_buff[512];
extern uint16_t usart2_cnt;

extern uint8_t lock_id[17];
extern uint8_t protocol_buff[512];



void protocol_analyze(void)
{
	mqtt_telegram_t mt;
	
	uint8_t topic_tmp[100] = {0};
	uint8_t topic[100] = {0};
	uint8_t buff[100] = {0};
	uint16_t data_len = 0;
	uint8_t *par = NULL;
	usart2_recv_data();
		
	//topic: lock/0018131408143929,1
	par = strstr((char *)protocol_buff, "topic:");
	
	if(par != NULL)
	{
		par = str_picked(protocol_buff, "topic:", ",", mt.topic);
		if(par != NULL)
		{
			
		}
	}
	
	sprintf((char*)topic, "%s%s", "lock/", lock_id);
	
//	if(strcmp(topic_tmp, topic) == 0)
	{
		
	}
	if(strstr((char *)usart2_buff, "+STATUS: MQTT CLOSE") != NULL)
	{
		
	}
	else if(strstr((char *)usart2_buff, (char*)topic) != NULL)
	{
		if(LOCK_ON_READ()==0 || LOCK_OFF_READ() == 0)
		{
			
			
		}
	}
}






















