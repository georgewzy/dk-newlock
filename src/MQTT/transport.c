/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "transport.h"
#include "usart.h"
#include "timer.h"
#include "gprs.h"
#include "list.h"



extern usart_buff_t usart1_rx_buff;
extern usart_buff_t usart2_rx_buff;
extern usart_buff_t mqtt_buff;
extern uint8_t mqtt_keep_alive_flag;
extern uint8_t mqtt_keep_alive_flag;
extern uint8_t mqtt_keep_alive_err_cnt;

int mqtt_buff_cnt = 0;
int mqtt_stauts = 0;

unsigned short mqtt_publish_msgid = 1;				//发布消息id


list_node *mqtt_recv_list = NULL;
list_node *mqtt_send_list = NULL;


//uint8_t mqtt_sub_buff[]
/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = -1;
	uint8_t *ret = NULL;
	int len = 0;
	uint8_t cmd[100] = {0};
	
	memset(cmd, 0, sizeof(cmd));
	sprintf((char *)cmd, "AT+CIPSEND=%d,1\r\n", buflen);
	ret = gprs_send_at(cmd, ">", 10, 30);
	if(ret != NULL)
	{
		timer_delay_1ms(20);
		usart_send_data(USART2, buf, buflen);
		usart_send_data(USART1, buf, buflen);
		rc = buflen;
	}
	
	return rc;
}


int transport_getdata(unsigned char* buf, int size)
{
	int rc = -1;
	
	if(mqtt_buff.index > 0)
	{
		USART_OUT(USART1, "transport_getdata=%d==%d\r\n", size,mqtt_buff.index);
		memcpy(buf, &mqtt_buff.pdata[mqtt_buff_cnt], size);
//		usart_send_data(USART1, mqtt_buff.pdata, mqtt_buff.index);
//		usart_send_data(USART1, &mqtt_buff.pdata[mqtt_buff_cnt], size);
		mqtt_buff_cnt += size;
		mqtt_buff.index = mqtt_buff.index - size;
		rc = size;
	}
	else
	{
		memset(&mqtt_buff, 0, sizeof(mqtt_buff));
		mqtt_buff_cnt = 0;
		rc = -1;
	}

	return rc;
}



int mqtt_connect(MQTTPacket_connectData *pdata)
{
	int ret = 0;
	int status = 0;	
	int rc = 0;
	int mysock = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int len = 0;
	int connect_status = CONNECT;
	
	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{ 
		usart2_recv_data();

		switch(connect_status)
		{
			case CONNECT:
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_connect(buf, buflen, pdata);
				if(len > 0)
				{					
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{
						connect_status = CONNACK;
						USART_OUT(USART1, "CONNECT\r\n");
					}
				}
			break;
			
			case CONNACK:
				if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
				{
					unsigned char sessionPresent, connack_rc;

					if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
					{
						USART_OUT(USART1, "Unable to connect, return code %d\n", connack_rc);
					}
					else
					{
						ret = 1;
						status = 1;
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);	
						USART_OUT(USART1, "CONNACK\r\n");
					}
				}
			break;
			
			default:
			break;	
		}	
		
		if(timer_is_timeout_1ms(timer_mqtt_timeout, 5000) == 0)
		{
			ret = 1;
			status = 0;
		}	
	}
	
	return status;
}


int mqtt_disconnect(void)
{
	int status = 0;
	int rc = 0;
	int mysock = 0;
	int len = 0;
	unsigned char buf[20];
	int buflen = sizeof(buf);
	
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);
	if(rc == 0)
	{
		status = 0;
	}
	else
	{
		status = 1;
	}
	
	return status;
}


int mqtt_publish(unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid)
{
	int status = 0;
	uint8_t ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200];
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	int publist_status = PUBLISH;
	uint8_t mqtt_publist_err_cnt = 0;
	int mqtt_msg_tpye = 0; 
	unsigned char type = 0;
	unsigned short msgid;
	unsigned char dup;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	timer_is_timeout_1ms(timer_mqtt_publist_timeout, 0);
	while(!ret)
	{
		usart2_recv_data();
		memset(buf, 0, sizeof(buf));
		mqtt_msg_tpye = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_msg_tpye > 0)
		{
			publist_status = mqtt_msg_tpye;
		}
		else
		{
			
		}
		
		switch(publist_status)
		{
			case PUBLISH:	
				timer_is_timeout_1ms(timer_mqtt_publist_timeout, 0);
				topicString.cstring = topic;				
//				strcpy(topicString.cstring, "test");	//死机
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_publish((unsigned char *)buf , buflen, 0, qos, 0, packetid, topicString, (unsigned char*)payload, payload_len);
				rc = transport_sendPacketBuffer(mysock, buf, len);	
				if(rc != -1)
				{
					publist_status = 0;
					USART_OUT(USART1, "PUBLISH=%d\r\n", packetid);
				}				
			break;
				
			case PUBREC:	
				rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);
				if(rc == 1)
				{
					if(packetid == msgid)
					{			
						USART_OUT(USART1, "PUBREC=%d\r\n", msgid);
								
						memset(buf, 0, sizeof(buf));
						len = MQTTSerialize_pubrel(buf, buflen, 0, msgid);
						rc = transport_sendPacketBuffer(mysock, buf, len);
						if(rc != -1)
						{
							publist_status = 0;
							USART_OUT(USART1, "PUBREL=%d\r\n", msgid);
						}
					}
				}										
			break;
				
			case PUBCOMP:
				rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);
				if(packetid == msgid)
				{		
					ret = 1;
					status = 1;
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);

					USART_OUT(USART1, "PUBCOMP=%d\r\n", msgid);	
		
	
					//消息id
					mqtt_publish_msgid++;
					if(mqtt_publish_msgid >= 65535)
					{
						mqtt_publish_msgid = 0;
					}
				}	
			break;
				
			default:
			break;	
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_publist_timeout, 5000) == 0)
		{
			publist_status = PUBLISH;
			USART_OUT(USART1, "=============================mqtt_publist_err_cnt\r\n");
			mqtt_publist_err_cnt++;
			if(mqtt_publist_err_cnt > 6)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "=============================mqtt_publist_err_cnt error\r\n");
			}

		}
	}
	
	return status;
}


int mqtt_subscribe(unsigned char* topic, unsigned char *payload, int *payloadlen)
{
	int status = 0;
	uint8_t ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200] = {0};	//数据过大容易导致死机 原因未知
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	unsigned char dup;
	int qos;
	unsigned char type = 0;
	unsigned short msgid;
	unsigned char retained;
	unsigned char* payload_in;
	MQTTString receivedTopic;
	uint8_t mqtt_subscribe_err_cnt = 0;
	int mqtt_msg_tpye = 0; 
	int subscribe_status = 0;
	int tmp = 0;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	timer_is_timeout_1ms(timer_mqtt_subscribe_timeout, 0);
	while(!ret)
	{	
		usart2_recv_data();
		memset(buf, 0, sizeof(buf));
		mqtt_msg_tpye = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_msg_tpye > 0)
		{
			if(mqtt_msg_tpye == PUBLISH || mqtt_msg_tpye == PUBREL)
			{
				tmp = 1; 
				subscribe_status = mqtt_msg_tpye;
			}
			switch(subscribe_status)
			{	
				case PUBLISH: //3
					rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, payloadlen, buf, buflen);
					if(rc == 1)
					{
						ret = 0;
						if(qos == 2)
						{
							memcpy(topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
							memcpy(payload, payload_in, *payloadlen);			
							
							USART_OUT(USART1, "Topic=%s======qos=%d\r\n", topic, qos);
							USART_OUT(USART1, "PUBLISH=%d\r\n", msgid);
							
							memset(buf, 0, sizeof(buf));
							len = MQTTSerialize_pubrec(buf, buflen, msgid);
							rc = transport_sendPacketBuffer(mysock, buf, len);
							if(rc != -1)
							{								
								USART_OUT(USART1, "PUBREC=%d\r\n", msgid);
							}	
						}	
						else if(qos == 1)
						{
							memcpy(topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
							memcpy(payload, payload_in, *payloadlen);			
							USART_OUT(USART1, "Topic=%s======qos=%d\r\n", topic, qos);
							USART_OUT(USART1, "PUBLISH=%d\r\n", msgid);
							
							memset(buf, 0, sizeof(buf));
							len = MQTTSerialize_puback(buf, buflen, msgid);
							rc = transport_sendPacketBuffer(mysock, buf, len);
							if(rc != -1)
							{
								timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);

								
								ret = 1;
								status = 1;

								USART_OUT(USART1, "PUBACK=%d\r\n", msgid);
							}
						}
					}		
				break;					
					
				case PUBREL:	//6	
					rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
					if(rc == 1)
					{		
						ret = 0;
						USART_OUT(USART1, "PUBREL=%d\r\n", msgid);	
						
						memset(buf, 0, sizeof(buf));
						len = MQTTSerialize_pubcomp(buf, buflen, msgid);
						rc = transport_sendPacketBuffer(mysock, buf, len);
						if(rc != -1)
						{					
							timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);

							ret = 1;
							status = 1;
							USART_OUT(USART1, "PUBCOMP=%d\r\n", msgid);
						}
						
					}
				break;
			
				default:
				break;	
			}	
		}
		else
		{
			if(tmp == 1)
			{
				ret = 0;
			}
			else
			{
				ret = 1;
			}
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_subscribe_timeout, 1000*30) == 0)
		{	
			USART_OUT(USART1, "========================================mqtt_subscribe_err_cnt\r\n");
			mqtt_subscribe_err_cnt++;
			if(mqtt_subscribe_err_cnt > 5)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "========================================mqtt_subscribe_err_cnt error\r\n");
			}
		}	
	}

	return status;
}




int mqtt_subscribe_topic(unsigned char* topic, int req_qos, unsigned short packetid)
{
	int status = 0;
	int ret = 0;
	unsigned char buf[200] = {0};
	int buflen = sizeof(buf);
	int len = 0;
	int rc = 0;
	int mysock = 0;
	uint8_t mqtt_subscribe_topic_err_cnt = 0;
	int subscribe_topic_status = SUBSCRIBE;
	int mqtt_msg_tpye = 0; 
	MQTTString topicString = MQTTString_initializer;
	unsigned short submsgid;
	int subcount;
	int granted_qos;
	
	timer_is_timeout_1ms(timer_mqtt_subscribe_topic_timeout, 0);
	while(!ret)
	{
		usart2_recv_data();
		memset(buf, 0, sizeof(buf));
		mqtt_msg_tpye = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_msg_tpye > 0)
		{			
			subscribe_topic_status = mqtt_msg_tpye;					
		}
	
		switch(subscribe_topic_status)
		{
			case SUBSCRIBE:
				/* subscribe */
				topicString.cstring = topic;
				memset(buf, 0, sizeof(buf));
				//subscribe send 
				len = MQTTSerialize_subscribe(buf, buflen, 0, packetid, 1, &topicString, &req_qos);
				if(len > 0)
				{
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{
						subscribe_topic_status = 0;
						USART_OUT(USART1, "SUBSCRIBE\r\n");
					}
				}
			break;
			
			case SUBACK:
				rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
				USART_OUT(USART1, "qos=%d==submsgid=%d\r\n", granted_qos, submsgid);
				if(rc == 1)
				{
					if(granted_qos == req_qos)
					{
						ret = 1;
						status = 1;
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
						//消息id
						mqtt_publish_msgid++;
						if(mqtt_publish_msgid >= 65535)
						{
							mqtt_publish_msgid = 0;
						}
						
						USART_OUT(USART1, "SUBACK\r\n");
					}
					else
					{
						ret = 1;
						status = 0;
						USART_OUT(USART1, "SUBACK error\r\n");
					}
		
				}
			break;
				
			default:
			break;	
			
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_subscribe_topic_timeout, 1000*5) == 0)
		{		
			topicString.cstring = topic;
			memset(buf, 0, sizeof(buf));
			//subscribe resend
			len = MQTTSerialize_subscribe(buf, buflen, 1, packetid, 1, &topicString, &req_qos);
			if(len > 0)
			{
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					subscribe_topic_status = 0;
					USART_OUT(USART1, "SUBSCRIBE\r\n");
				}
			}
				
//			subscribe_topic_status = SUBSCRIBE;
			USART_OUT(USART1, "=============================mqtt_subscribe_topic_err_cnt\r\n");
			mqtt_subscribe_topic_err_cnt++;
			if(mqtt_subscribe_topic_err_cnt > 6)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "=============================mqtt_subscribe_topic_err_cnt error\r\n");
			}
		}
	}
	
	return status;
}


int mqtt_unsubscribe_topic(unsigned char* topic, int req_qos, unsigned short packetid)
{
	
}




int mqtt_keep_alive1(uint32_t ms)
{
	int status = 0;
	int ret = 0;
	int rc = 0;
	int mysock = 0;
	int len = 0;
	unsigned char buf[20];
	int buflen = sizeof(buf);
	uint8_t keep_alive_err_cnt = 0;
	unsigned char type = 0;
	unsigned short msgid;
	int mqtt_msg_tpye = 0; 
	int keep_alive_status = PINGREQ;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);
	while(!ret)
	{	
		usart2_recv_data();	
		mqtt_msg_tpye = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_msg_tpye > 0)
		{
			keep_alive_status = mqtt_msg_tpye;
		}
		else
		{
			
		}
		
		switch(keep_alive_status)
		{					
			case PINGREQ:		
				timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);
				len = MQTTSerialize_pingreq(buf, buflen);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				keep_alive_status = 0;
				USART_OUT(USART1, " PINGREQ\r\n");
			break;
			
			case PINGRESP:
				rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
				if(rc == 1)
				{
					ret = 1;
					status = 1;
					
					USART_OUT(USART1, " PINGRESP\r\n");
					USART_OUT(USART1, "message ack type=%d==msgid=%d\n", type, msgid);
				}		
			break;
			
			default:
			break;					
		}			

		if(timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 5000) == 0)
		{		
			keep_alive_status = PINGREQ;
			USART_OUT(USART1, "=============================keep_alive_err_cnt\r\n");
			keep_alive_err_cnt++;
			if(keep_alive_err_cnt > 6)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "============================keep_alive_err_cnt error\r\n");
			}
		}
	}
	
	return status;
}


int mqtt_publish_qos0(unsigned char* topic, unsigned char* payload, int payload_len)
{
	int status = 0;
	uint8_t ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200] = {0};
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	int publist_status = PUBLISH;

	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	memset(buf, 0, sizeof(buf));
	topicString.cstring = topic;
	len = MQTTSerialize_publish((unsigned char *)buf , buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payload_len);
	if(len > 0 )
	{
//		gprs_wakeup_status = gprs_wakeup(0);
//		if(gprs_wakeup_status == 1)
		{
			USART_OUT(USART1, "mqtt_publist_qos0_gprs_wakeup=%d\r\n", len);
			rc = transport_sendPacketBuffer(mysock, buf, len);	
			if(rc != -1)
			{
				status = 1;
			}
		}
			
	}
			
	return status;
}


int mqtt_keep_alive_test(void)
{
	int status = 0;
	int ret = 0;
	int rc = 0;
	int mysock = 0;
	int len = 0;
	unsigned char buf[20];
	int buflen = sizeof(buf);
	uint8_t keep_alive_err_cnt = 0;
	unsigned char type = 0;
	unsigned short msgid;
	int mqtt_msg_tpye = 0; 
	int keep_alive_status = PINGREQ;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);
	while(!ret)
	{	
		usart2_recv_data();	
		mqtt_msg_tpye = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_msg_tpye > 0)
		{
			keep_alive_status = mqtt_msg_tpye;
		}
		else
		{
			
		}
		
		switch(keep_alive_status)
		{					
			case PINGREQ:		
				timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 0);
				len = MQTTSerialize_pingreq(buf, buflen);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				keep_alive_status = 0;
				USART_OUT(USART1, " PINGREQ\r\n");
			break;
			
			case PINGRESP:
				rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
				if(rc == 1)
				{
					ret = 1;
					status = 1;
					
					USART_OUT(USART1, " PINGRESP\r\n");
					USART_OUT(USART1, "message ack type=%d==msgid=%d\n", type, msgid);
				}		
			break;
			
			default:
			break;					
		}			

		if(timer_is_timeout_1ms(timer_mqtt_keep_alive_timeout, 5000) == 0)
		{		
			keep_alive_status = PINGREQ;
			USART_OUT(USART1, "=============================keep_alive_err_cnt\r\n");
			keep_alive_err_cnt++;
			if(keep_alive_err_cnt > 6)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "============================keep_alive_err_cnt error\r\n");
			}
		}
	}
	
	return status;
}


int mqtt_subscribe_qos0(unsigned char* topic, unsigned char *payload, int *payloadlen)
{
	int status = 0;
	uint8_t ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200] = {0};	//数据过大容易导致死机 原因未知
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	unsigned char dup;
	int qos;
	unsigned char type = 0;
	unsigned short msgid;
	unsigned char retained;
	unsigned char* payload_in;
	MQTTString receivedTopic;
	uint8_t mqtt_subscribe_err_cnt = 0;
	int mqtt_msg_tpye = 0; 
	int subscribe_status = 0;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	usart2_recv_data();
	if(MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK)
	{
		rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, payloadlen, buf, buflen);
		if(rc == 1)
		{
			memcpy(topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
			memcpy(payload, payload_in, *payloadlen);			
			
			USART_OUT(USART1, "Topic=%s======qos=%d\r\n", topic, qos);
			USART_OUT(USART1, "PUBLISH qos0\r\n");
		}	
	}
}



int mqtt_publish_qos2(list_node **list, unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid)
{
	int size = 0;
	int status = -1;
	int rc = 0;
	int len = 0;
	char buf[200];
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s *msg = NULL;
	

	topicString.cstring = topic;	
	
//	msg = (mqtt_msg_s *)malloc(sizeof(mqtt_msg_s));
//	if(msg == NULL)
//	{
//		return status;
//	}
	
	memset(msg, 0 , sizeof(mqtt_msg_s));	
	memset(&mqtt_msg, 0, sizeof(mqtt_msg));
	
	memcpy(mqtt_msg.topic, topic, strlen(topic));
	memcpy(mqtt_msg.payload, payload, payload_len);
	mqtt_msg.payloadlen = payload_len;
	mqtt_msg.dup = 0;
	mqtt_msg.qos = qos;
	mqtt_msg.msg_id = packetid;
	mqtt_msg.status = PUBLISH;	
	
		//消息id
	mqtt_publish_msgid++; 
	if(mqtt_publish_msgid >= 65535)
	{
		mqtt_publish_msgid = 0;
	}
	
	size = list_size(list);	//链表的大小
	if(size == 0)	//链表为空
	{
		USART_OUT(USART1, "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\r\n");
		list_insert_last(list, mqtt_msg);
		list_send_travese(list);
		status = 1;
		USART_OUT(USART1, "publisher_packetid_repeat1=%d\r\n", packetid);
	}
	else if(size > 0 && size < 5)	//链表不为空
	{
		msg = list_get_addr_by_msgid(list, packetid); 	//是否链表中存在此id			
		if(msg == NULL)
		{
			USART_OUT(USART1, "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\r\n");
			list_insert_last(list, mqtt_msg);
			list_send_travese(list);
			status = 1;
			USART_OUT(USART1, "publisher packetid repeat2=%d\r\n", packetid);
			
		}
		else
		{
			USART_OUT(USART1, "publisher packetid repeat3=%d\r\n", packetid);
			USART_OUT(USART1, "publisher msgid insert2=%d\r\n", msg->msg_id);
		}
		
	}
	else	//超过链表的长度 删除一个旧节点重新插入新节点
	{
		msg = list_find_min_val(list);
		if(msg != NULL)
		{
			USART_OUT(USART1, "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\r\n");
			list_de_by_msgid(list, msg->msg_id);	//删除旧节点
			list_insert_last(list, mqtt_msg);
			list_send_travese(list);
			status = 1;
			USART_OUT(USART1, "publisher_msgid_delete =%d\r\n", msg->msg_id);
		}
	}

//	free(msg);
	return status;
}


int mqtt_client(list_node **list_recv, list_node **list_send, uint8_t msg_tpye)
{
	int i = 0;
	int rc = 0;
	int len = 0;
	char buf[200];
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	unsigned char retained;
	unsigned char* payload_in;
	int payloadlen_in;
	MQTTString receivedTopic;
	unsigned char type = 0;
	unsigned short msgid;
	unsigned char dup;
	int qos = 0;
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	int publisher = 0;
	int list_send_empty_status = 0;
	int list_send_size = 0;
	int list_recv_size = 0;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s *msg1 = NULL;
	list_node *tmp1 = *list_send;
	list_node *tmp2 = *list_send;
	
	memset(buf, 0, sizeof(buf));
	memset(&mqtt_msg, 0, sizeof(mqtt_msg));	
	
	if(msg_tpye != MQTTNULL)
	{
		mqtt_stauts = msg_tpye;
	}	
	else 	//不为空
	{
		
		memset(buf, 0, sizeof(buf));
		mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
		if(mqtt_stauts > 0)
		{
			if(mqtt_stauts == PUBLISH)
			{
				publisher = 0;
			}	
		}			
		else
		{
			list_send_empty_status = list_is_empty(list_send);	
			if(list_send_empty_status == 1)//链表不为空
			{
				msg1 = list_get_addr_by_status(list_send, PUBLISH);	//找到状态为PUBLISH的节点
				if(msg1 != NULL)	//找到状态为PUBLISH的节点
				{
					if(msg1->status == PUBLISH)
					{
						publisher = 1;
						mqtt_stauts = msg1->status;
						timer_send_list_pubrec_1ms(msg1->msg_id%5, 0);//加入时间标签
						USART_OUT(USART1, "timer_send_list_1ms_aa=%d\r\n", tmp1->msg.msg_id);
					}
				}
				else	//找不到状态为PUBLISH的节点
				{
					memset(buf, 0, sizeof(buf));
					mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
					publisher = 0;		
				}			
				//重复PUBLISH
				while(tmp1 != NULL)
				{	
					if(tmp1->msg.status == PUBREC)
					{
						if(timer_send_list_pubrec_1ms(tmp1->msg.msg_id%5, 1000*10) == 0)
						{
							topicString.cstring = tmp1->msg.topic;
							memset(buf, 0, sizeof(buf));
							USART_OUT(USART1, "tttttttttttttttt\r\n");
							//publisher resend publish dup=1
							list_send_travese(list_send);
							len = MQTTSerialize_publish((unsigned char*)buf, buflen, 1, tmp1->msg.qos, 0, tmp1->msg.msg_id, topicString, tmp1->msg.payload, tmp1->msg.payloadlen);
							if(len > 0)
							{	
								usart_send_data(USART1, buf, len);
								rc = transport_sendPacketBuffer(mysock, buf, len);	//publisher	publish
								if(rc != -1)
								{
									USART_OUT(USART1, "bbbbbbbbbbbbbbb=%d==%d\r\n", tmp1->msg.msg_id, len);
									list_modify_status(list_send, tmp1->msg.msg_id, PUBREC);
									list_send_travese(list_send);
									USART_OUT(USART1, "publisher_PUBLISH_resend=%d\r\n", tmp1->msg.msg_id);
								}				
							}			
						}
					}
					tmp1 = tmp1->next;
				}
				
				while(tmp2 != NULL)
				{	
					if(tmp2->msg.status == PUBREL)
					{
						if(timer_send_list_pubrel_1ms(tmp2->msg.msg_id%5, 1000*10) == 0)	//重新发送
						{
							USART_OUT(USART1, "timer_send_list_1ms_cc11=%d\r\n", tmp2->msg.msg_id);
							USART_OUT(USART1, "timer_send_list_1ms_cc22=%d\r\n", tmp2->msg.msg_id%5);
							
							memset(buf, 0, sizeof(buf));
							len = MQTTSerialize_pubrel(buf, buflen, 1, tmp2->msg.msg_id);	//publisher resend pubrel dup=1
							if(len > 0)
							{
								USART_OUT(USART1, "publisher_4444444444=%d\r\n", len);
								rc = transport_sendPacketBuffer(mysock, buf, len);
								USART_OUT(USART1, "publisher_3333333333=%d\r\n", rc);
								if(rc != -1)
								{
									list_modify_status(list_send, tmp2->msg.msg_id, PUBREL);
									list_send_travese(list_send);			
									USART_OUT(USART1, "publisher_PUBREL_resend=%d\r\n", tmp2->msg.msg_id);
								}	
							}						
						}
					}
					tmp2 = tmp2->next;
				}	
				
			}
		}
/*		
		list_send_empty_status = list_is_empty(list_send);	
		if(list_send_empty_status == 1)//链表不为空
		{
			msg1 = list_get_addr_by_status(list_send, PUBLISH);	//找到状态为PUBLISH的节点
			if(msg1 != NULL)	//找到状态为PUBLISH的节点
			{
				if(msg1->status == PUBLISH)
				{
					publisher = 1;
					mqtt_stauts = msg1->status;
					timer_send_list_pubrec_1ms(msg1->msg_id%5, 0);//加入时间标签
					USART_OUT(USART1, "timer_send_list_1ms_aa=%d\r\n", tmp1->msg.msg_id);
				}
			}
			else	//找不到状态为PUBLISH的节点
			{
				memset(buf, 0, sizeof(buf));
				mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
				publisher = 0;		
			}			
			//重复PUBLISH
			while(tmp1 != NULL)
			{	
				if(tmp1->msg.status == PUBREC)
				{
					if(timer_send_list_pubrec_1ms(tmp1->msg.msg_id%5, 1000*10) == 0)
					{
						topicString.cstring = tmp1->msg.topic;
						memset(buf, 0, sizeof(buf));
						USART_OUT(USART1, "tttttttttttttttt\r\n");
						//publisher resend publish dup=1
						list_send_travese(list_send);
						len = MQTTSerialize_publish((unsigned char*)buf, buflen, 1, tmp1->msg.qos, 0, tmp1->msg.msg_id, topicString, tmp1->msg.payload, tmp1->msg.payloadlen);
						if(len > 0)
						{	
							usart_send_data(USART1, buf, len);
							rc = transport_sendPacketBuffer(mysock, buf, len);	//publisher	publish
							if(rc != -1)
							{
								USART_OUT(USART1, "bbbbbbbbbbbbbbb=%d==%d\r\n", tmp1->msg.msg_id, len);
								list_modify_status(list_send, tmp1->msg.msg_id, PUBREC);
								list_send_travese(list_send);
								USART_OUT(USART1, "publisher_PUBLISH_resend=%d\r\n", tmp1->msg.msg_id);
							}				
						}			
					}
				}
				tmp1 = tmp1->next;
			}
			
			while(tmp2 != NULL)
			{	
				if(tmp2->msg.status == PUBREL)
				{
					if(timer_send_list_pubrel_1ms(tmp2->msg.msg_id%5, 1000*10) == 0)	//重新发送
					{
						USART_OUT(USART1, "timer_send_list_1ms_cc11=%d\r\n", tmp2->msg.msg_id);
						USART_OUT(USART1, "timer_send_list_1ms_cc22=%d\r\n", tmp2->msg.msg_id%5);
						
						memset(buf, 0, sizeof(buf));
						len = MQTTSerialize_pubrel(buf, buflen, 1, tmp2->msg.msg_id);	//publisher resend pubrel dup=1
						if(len > 0)
						{
							USART_OUT(USART1, "publisher_4444444444=%d\r\n", len);
							rc = transport_sendPacketBuffer(mysock, buf, len);
							USART_OUT(USART1, "publisher_3333333333=%d\r\n", rc);
							if(rc != -1)
							{
								list_modify_status(list_send, tmp2->msg.msg_id, PUBREL);
								list_send_travese(list_send);			
								USART_OUT(USART1, "publisher_PUBREL_resend=%d\r\n", tmp2->msg.msg_id);
							}	
						}						
					}
				}
				tmp2 = tmp2->next;
			}	
			
		}
		else //链表为空
		{	
			memset(buf, 0, sizeof(buf));
			mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
			publisher = 0;		
		}
*/		
	}

	
	switch(mqtt_stauts)
	{
		case CONNECT:
			
		break;
		
		case CONNACK:
			
		break;
		
		case PUBLISH:
			if(publisher == 1)
			{			
				publisher = 0;
				topicString.cstring = msg1->topic;
				memset(buf, 0, sizeof(buf));
//				topicString.cstring = (*list_send)->msg.topic;	
//				len = MQTTSerialize_publish((unsigned char*)buf ,buflen, 0, (*list_send)->msg.qos, 0, (*list_send)->msg.msg_id, topicString, (unsigned char*)(*list_send)->msg.payload, (*list_send)->msg.payloadlen);
				//publisher publish dup=0
				len = MQTTSerialize_publish((unsigned char*)buf ,buflen, 0, msg1->qos, 0, msg1->msg_id, topicString, msg1->payload, msg1->payloadlen);
				if(len > 0)
				{			
//					gprs_wakeup_status = gprs_wakeup(0);
//					if(gprs_wakeup_status == 1)
					{
						USART_OUT(USART1, "publisher_gprs_wakeup_ok\r\n");
						rc = transport_sendPacketBuffer(mysock, buf, len);	//publisher	publish
						if(rc != -1)
						{
							USART_OUT(USART1, "AAAAAAAAAA=%d\r\n", msg1->msg_id);
							list_modify_status(list_send, msg1->msg_id, PUBREC);
							list_send_travese(list_send);
							USART_OUT(USART1, "publisher_PUBLISH=%d\r\n", msg1->msg_id);
						}	
					}			
				}
			}
			else
			{
				rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
				&payload_in, &payloadlen_in, buf, buflen);		//subscriber publish
				if(rc == 1)
				{
					
					if(qos == 2)
					{	
						USART_OUT(USART1, "subscriber_PUBLISH=%d\r\n", msgid);
						memset(&mqtt_msg, 0, sizeof(mqtt_msg));
						memcpy(mqtt_msg.topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
						memcpy(mqtt_msg.payload, payload_in, payloadlen_in);
						mqtt_msg.payloadlen = payloadlen_in;
						mqtt_msg.retained = retained;
						mqtt_msg.dup = dup;
						mqtt_msg.qos = qos;
						mqtt_msg.msg_id = msgid;
						mqtt_msg.status = PUBREC;
										
						list_recv_size = list_size(list_recv);
						USART_OUT(USART1, "list_recv_size11=%d\r\n", list_recv_size);	
						if(list_recv_size == 0)
						{
							USART_OUT(USART1, "msgid22=%d\r\n", msgid);		
							USART_OUT(USART1, "subscriber_frist_insert\r\n");
							list_insert_last(list_recv, mqtt_msg);
							list_travese(list_recv);
						}
						else if(list_recv_size > 0 && list_recv_size < 5)
						{	
							msg1 = list_get_addr_by_msgid(list_recv, msgid);
							if(msg1 != NULL)
							{
								USART_OUT(USART1, "subscriber_msgid_repeat=%d\r\n", msgid);
								USART_OUT(USART1, "subscriber_msgid_insert=%d\r\n", msg1->msg_id);
							}
							else
							{
								USART_OUT(USART1, "msg1=%d\r\n", msg1);	
								USART_OUT(USART1, "msgid11=%d\r\n", msgid);		
								USART_OUT(USART1, "msg->msg_id and msgid no\r\n");
								list_travese(list_recv);
								USART_OUT(USART1, "msg->msg_id and msgid no22\r\n");
								list_insert_last(list_recv, mqtt_msg);
								list_travese(list_recv);		
							}
						}
						else	//超过链表的长度 删除一个旧节点重新插入新节点
						{
							msg1 = list_find_min_val(list_recv);
							if(msg1 != NULL)
							{
								USART_OUT(USART1, "list_find_min_val=%d\r\n", msg1->msg_id);
								list_de_by_msgid(list_recv, msg1->msg_id);	//删除
								list_insert_last(list_recv, mqtt_msg);
								list_travese(list_recv);
								USART_OUT(USART1, "subscriber_msgid_delete =%d\r\n", msg1->msg_id);
							}
						}
						
						////
						memset(buf, 0, sizeof(buf));
						len = MQTTSerialize_pubrec(buf, buflen, msgid);		//subscriber pubrec
						if(len > 0)
						{
							rc = transport_sendPacketBuffer(mysock, buf, len);
							if(rc != -1)
							{	
								USART_OUT(USART1, "subscriber_PUBREC=%d\r\n", msgid);
							}	
						}
					}		
				}	
			}
		break;
	
		case PUBACK:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				USART_OUT(USART1, " PUBACK=%d\r\n", msgid);					
			}			
		break;
		
		case PUBREC:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);	//publisher	pubrec
			if(rc == 1)
			{
				USART_OUT(USART1, "publisher_PUBREC=%d\r\n", msgid);				
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubrel(buf, buflen, 0, msgid);	//publisher  pubrel dup=0
				if(len > 0)
				{
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{
						list_modify_status(list_send, msgid, PUBREL);
						timer_send_list_pubrel_1ms(msgid%5, 0);//加入时间标签
	//					list_travese(list_send);			
						USART_OUT(USART1, "publisher_PUBREL=%d\r\n", msgid);
					}	
				}
			}	
		break;
			
		case PUBREL:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);	//subscriber pubrel
			if(rc == 1)
			{		
				USART_OUT(USART1, "subscriber_PUBREL=%d\r\n", msgid);					
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubcomp(buf, buflen, msgid);
				if(len > 0)
				{	
					rc = transport_sendPacketBuffer(mysock, buf, len);	//subscriber pubcomp
					if(rc != -1)
					{	
						list_modify_status(list_recv, msgid, PUBCOMP);
	//					list_travese(list_recv);
						USART_OUT(USART1, "subscriber_PUBCOMP=%d\r\n", msgid);
						
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					
//						if((list_is_empty(list_send) == NULL) && (list_is_empty(list_recv) == NULL))	//发送和接收队列都为空
//						{
//							gprs_sleep_status = gprs_sleep();		// 睡眠
//							if(gprs_sleep_status == 1)
//							{
//								USART_OUT(USART1, "subscriber_gprs_sleep_ok\r\n");	
//							}	
//						}
					}
				}				
			}
		break;
		
		case PUBCOMP:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);

				USART_OUT(USART1, "publisher_PUBCOMP=%d\r\n", msgid);
				
				msg1 = list_get_addr_by_msgid(list_send ,msgid);
				if(msg1 != NULL)
				{
					if(msg1->msg_id == msgid)
					{
						USART_OUT(USART1, "publisher_find_msgid\r\n");
						list_de_by_msgid(list_send, msg1->msg_id);	//从链表中删除节点
						list_send_travese(list_send);
					}
				}	
				
//				if((list_is_empty(list_send) == -1) && (list_is_empty(list_recv) == -1)) //发送和接收队列都为空
//				{
//					gprs_sleep_status = gprs_sleep();
//					if(gprs_sleep_status == 1)
//					{
//						USART_OUT(USART1, "publisher_gprs_sleep_ok\r\n");	
//					}
//				}
				
			}	
		break;
		
		case SUBSCRIBE:
			
		break;
		
		case SUBACK:
			
		break;
		
		case UNSUBSCRIBE:
			
		break;
		
		case UNSUBACK:
			
		break;
		
		case PINGREQ:
			memset(buf, 0, sizeof(buf));
			len = MQTTSerialize_pingreq(buf, buflen);
			if(len > 0)
			{
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					USART_OUT(USART1, "PINGREQ\r\n");
				}
			}
		break;
		
		case PINGRESP:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
			if(rc == 1)
			{
//				gprs_sleep_status = gprs_sleep();
//				if(gprs_sleep_status == 1)
//				{
//					USART_OUT(USART1, "keep_alive_gprs_sleep_ok\r\n");	
//				}
				
				mqtt_keep_alive_flag = 0;
				mqtt_keep_alive_err_cnt = 0;
				USART_OUT(USART1, "PINGRESP\r\n");
			}		
		break;
		
		case DISCONNECT:
			
		break;
		
		default:
		break;
	}
	
	
	return mqtt_stauts;
}











