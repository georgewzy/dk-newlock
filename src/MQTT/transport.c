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
extern uint8_t send_buff[100];
extern uint8_t mqtt_keep_alive_flag;





int mqtt_buff_cnt = 0;
int mqtt_stauts = 0;


unsigned short mqtt_publist_msgid = 1;				//发布消息id
unsigned short mqtt_subscribe_msgid = 1;				//订阅消息id

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
	uint8_t *ret;
	int len = 0;
	uint8_t cmd[50] = {0};
	
	memset(send_buff, 0, sizeof(send_buff));
	memset(&mqtt_buff, 0, sizeof(mqtt_buff));
	mqtt_buff_cnt = 0;
	sprintf((char *)cmd, "AT+CIPSEND=%d,1\r\n", buflen);
	ret = gprs_send_at(cmd, ">", 10, 30);
	if(ret != NULL)
	{
		timer_delay_1ms(20);
		memcpy((char*)send_buff, buf, buflen);
		usart_send_data(USART2, buf, buflen);
//		USART_OUT(USART1, "BBBB");
//		usart_send_data(USART1, send_buff, buflen);		
//		USART_OUT(USART1, "BBBB");
		rc = buflen;
	}
	
	return rc;
}


int transport_getdata(unsigned char* buf, int size)
{
	int rc = -1;
	
	if(mqtt_buff.index > 0)
	{
		memcpy(buf, &mqtt_buff.pdata[mqtt_buff_cnt], size);
//		usart_send_data(USART1, mqtt_buff.pdata, mqtt_buff.index);
//		usart_send_data(USART1, &mqtt_buff.pdata[mqtt_buff_cnt], size);
		mqtt_buff_cnt += size;
		rc = size;
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
				rc = transport_sendPacketBuffer(mysock, buf, len);
				connect_status = CONNACK;
				USART_OUT(USART1, "CONNECT\r\n");
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
				gprs_wakeup_status = gprs_wakeup(0);
				if(gprs_wakeup_status == 1)
				{
					USART_OUT(USART1, "publist gprs_wakeup ok\r\n");
				}
				
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
					timer_is_timeout_1ms(timer_heartbeat, 0);
					USART_OUT(USART1, "PUBCOMP=%d\r\n", msgid);	
					
					gprs_sleep_status = gprs_sleep();
					if(gprs_sleep_status == 1)
					{
						USART_OUT(USART1, "publist gprs_sleep ok\r\n");	
					}
					//消息id
					mqtt_publist_msgid++;
					if(mqtt_publist_msgid >= 65535)
					{
						mqtt_publist_msgid = 0;
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
								timer_is_timeout_1ms(timer_heartbeat, 0);
								gprs_sleep_status = gprs_sleep();
								if(gprs_sleep_status == 1)
								{
									USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
								}
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
							timer_is_timeout_1ms(timer_heartbeat, 0);
							gprs_sleep_status = gprs_sleep();
							if(gprs_sleep_status == 1)
							{
								USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
							}

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
				len = MQTTSerialize_subscribe(buf, buflen, 0, packetid, 1, &topicString, &req_qos);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				subscribe_topic_status = 0;
				USART_OUT(USART1, "SUBSCRIBE\r\n");
			break;
			
			case SUBACK:
				rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
				USART_OUT(USART1, "qos=%d==submsgid=%d\r\n", granted_qos, submsgid);
				if(rc == 1)
				{
					if(submsgid == packetid && granted_qos == req_qos)
					{
						ret = 1;
						status = 1;
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
						//消息id
						mqtt_publist_msgid++;
						if(mqtt_publist_msgid >= 65535)
						{
							mqtt_publist_msgid = 0;
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
		
		if(timer_is_timeout_1ms(timer_mqtt_subscribe_topic_timeout, 5000) == 0)
		{
			subscribe_topic_status = SUBSCRIBE;
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




int mqtt_keep_alive(uint32_t ms)
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
					gprs_sleep_status = gprs_sleep();
					if(gprs_sleep_status == 1)
					{
						USART_OUT(USART1, "keep_alive gprs_sleep ok\r\n");	
					}
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


int mqtt_publist_qos0(unsigned char* topic, unsigned char* payload, int payload_len)
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

	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	
	len = MQTTSerialize_publish((unsigned char *)buf , buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payload_len);
	rc = transport_sendPacketBuffer(mysock, buf, len);	
	if(rc != -1)
	{
		status = 1;
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
					gprs_sleep_status = gprs_sleep();
					if(gprs_sleep_status == 1)
					{
						USART_OUT(USART1, "keep_alive gprs_sleep ok\r\n");	
					}
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
//	topicString.cstring = (*list);	
	
//	memset(buf, 0, sizeof(buf));
//	len = MQTTSerialize_publish((unsigned char*)buf , buflen, 0, qos, 0, packetid, topicString, (unsigned char*)payload, payload_len);
//	if(len > 0)
//	{		
		memset(&mqtt_msg, 0, sizeof(mqtt_msg));
		
		memcpy(mqtt_msg.topic, topic, strlen(topic));
		memcpy(mqtt_msg.payload, payload, payload_len);
		mqtt_msg.payloadlen = payload_len;
		mqtt_msg.dup = 0;
		mqtt_msg.qos = qos;
		mqtt_msg.msg_id = packetid;
		mqtt_msg.status = PUBLISH;	
			
		size = list_size(*list);
		if(size <= 5)
		{
			msg = list_get_addr_by_msgid(*list, packetid);
			if(msg->msg_id != packetid)
			{
				list_insert_last(list, mqtt_msg);
				list_travese(list);
				status = 1;
			}
			else
			{
				USART_OUT(USART1, "subscriber packetid repeat =%d\r\n", packetid);
			}
		}
		else
		{
			msg = list_find_min_val(list);
			list_de_by_elem(list, msg->msg_id);
			list_insert_last(list, mqtt_msg);
			list_travese(list);
			status = 1;
			USART_OUT(USART1, "subscriber msgid delete =%d\r\n", msg->msg_id);
		}

//		gprs_wakeup_status = gprs_wakeup(0);
//		if(gprs_wakeup_status == 1)
//		{
//			USART_OUT(USART1, "publist gprs_wakeup ok\r\n");
//			rc = transport_sendPacketBuffer(mysock, buf, len);	
//			if(rc != -1)
//			{
//				status = 1;
//				USART_OUT(USART1, "publisher PUBLISH=%d\r\n", packetid);
//			}	
//		}			
//	}
	
	return status;
}


int mqtt_client(list_node **list_recv, list_node **list_send, uint8_t msg_tpye)
{
	int size = 0;
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
	int mqtt_msg_tpye = 0; 
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	int publisher = 0;
	int list_send_status = 0;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s *msg = NULL;
	
	memset(buf, 0, sizeof(buf));
		
	list_send_status = list_is_empty(list_send);
	
	if(msg_tpye != MQTTNULL)
	{
		mqtt_stauts = msg_tpye;
	}	
	else if(list_send_status == 1)	//不为空
	{
		msg = list_get_addr_by_status(*list_send, PUBLISH);
		if(msg->status == PUBLISH)
		{
			publisher = 1;
			mqtt_stauts = msg->status;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
			publisher = 0;
		}
	}
//	else if(msg_tpye != MQTTNULL)
//	{
//		mqtt_stauts = msg_tpye;
//	}
	else
	{	
		memset(buf, 0, sizeof(buf));
		mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
		publisher = 0;
	}
	
	switch(mqtt_stauts)
	{
		case CONNECT:
			
		break;
		
		case CONNACK:
			
		break;
		
		case PUBLISH:
			timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
//			mqtt_keep_alive_flag = 0;
			if(publisher == 1)
			{										
				topicString.cstring = (*list_send)->msg.topic;	
				
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_publish((unsigned char*)buf , buflen, 0, (*list_send)->msg.qos, 0, (*list_send)->msg.msg_id, topicString, (unsigned char*)(*list_send)->msg.payload, (*list_send)->msg.payloadlen);
				if(len > 0)
				{			
					gprs_wakeup_status = gprs_wakeup(0);
					if(gprs_wakeup_status == 1)
					{
						USART_OUT(USART1, "publisher gprs_wakeup ok\r\n");
						rc = transport_sendPacketBuffer(mysock, buf, len);	
						if(rc != -1)
						{
							list_modify_elem(list_send, (*list_send)->msg.msg_id, PUBREC);
							USART_OUT(USART1, "publisher PUBLISH=%d\r\n", (*list_send)->msg.msg_id);
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
					USART_OUT(USART1, "subscriber PUBLISH=%d\r\n", msgid);
					memset(&mqtt_msg, 0, sizeof(mqtt_msg));
					
					memcpy(mqtt_msg.topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
					memcpy(mqtt_msg.payload, payload_in, payloadlen_in);
					mqtt_msg.payloadlen = payloadlen_in;
					mqtt_msg.retained = retained;
					mqtt_msg.dup = dup;
					mqtt_msg.qos = qos;
					mqtt_msg.msg_id = msgid;
					mqtt_msg.status = PUBREC;
									
					size = list_size(*list_recv);
					if(size <= 5)
					{
						msg = list_get_addr_by_msgid(*list_send ,msgid);
						if(msg->msg_id != msgid)
						{
							list_insert_last(list_recv, mqtt_msg);
							list_travese(list_recv);
						}
						else
						{
							USART_OUT(USART1, "subscriber msgid repeat =%d\r\n", msgid);
						}
					}
					else
					{
						msg = list_find_min_val(list_recv);
						list_de_by_elem(list_recv, msg->msg_id);
						list_insert_last(list_recv, mqtt_msg);
						list_travese(list_recv);
						USART_OUT(USART1, "subscriber msgid delete =%d\r\n", msg->msg_id);
					}
					USART_OUT(USART1, "list_size=%d\r\n", size);
					////
					memset(buf, 0, sizeof(buf));
					len = MQTTSerialize_pubrec(buf, buflen, msgid);		//subscriber pubrec
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{	
						USART_OUT(USART1, "subscriber PUBREC=%d\r\n", msgid);
					}	
				}	
				else if(qos == 1)
				{
					memset(buf, 0, sizeof(buf));
					len = MQTTSerialize_puback(buf, buflen, msgid);
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
						timer_is_timeout_1ms(timer_heartbeat, 0);
						USART_OUT(USART1, "subscriber PUBACK=%d\r\n", msgid);
					}
				}
			}	
			}
		break;
	
		case PUBACK:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
				timer_is_timeout_1ms(timer_heartbeat, 0);
				USART_OUT(USART1, " PUBCOMP=%d\r\n", msgid);	
				
			}	
			
		break;
		
		case PUBREC:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);	//publisher	pubrec
			if(rc == 1)
			{
				USART_OUT(USART1, "publisher PUBREC=%d\r\n", msgid);
				
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubrel(buf, buflen, 0, msgid);	//publisher  pubrel
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					list_modify_elem(list_send, msgid, PUBREL);
//					list_travese(list_send);
					
					USART_OUT(USART1, "publisher PUBREL=%d\r\n", msgid);
				}	
			}	
		break;
			
		case PUBREL:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);	//subscriber pubrel
			if(rc == 1)
			{		
				USART_OUT(USART1, "subscriber PUBREL=%d\r\n", msgid);	
				
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubcomp(buf, buflen, msgid);	
				rc = transport_sendPacketBuffer(mysock, buf, len);	//subscriber pubcomp
				if(rc != -1)
				{	
					list_modify_elem(list_recv, msgid, PUBCOMP);
//					list_travese(list_recv);
					
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					timer_is_timeout_1ms(timer_heartbeat, 0);
					
//					list_empty_status = list_is_empty(list_recv);
					
					gprs_sleep_status = gprs_sleep();		// 睡眠
					if(gprs_sleep_status == 1)
					{
						USART_OUT(USART1, "subscriber gprs_sleep ok\r\n");	
					}
					
					USART_OUT(USART1, "subscriber PUBCOMP=%d\r\n", msgid);
				}	
			}
		break;
		
		case PUBCOMP:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
				timer_is_timeout_1ms(timer_heartbeat, 0);
				USART_OUT(USART1, "publisher PUBCOMP=%d\r\n", msgid);
				
				
				msg = list_get_addr_by_msgid(*list_send ,msgid);
				if(msg->msg_id == msgid)
				{
					list_de_by_elem(list_send, msg->msg_id);
					list_travese(list_send);
				}
							
				gprs_sleep_status = gprs_sleep();
				if(gprs_sleep_status == 1)
				{
					USART_OUT(USART1, "publisher gprs_sleep ok\r\n");	
				}
				//消息id
				mqtt_publist_msgid++;
				if(mqtt_publist_msgid >= 65535)
				{
					mqtt_publist_msgid = 0;
				}
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
			rc = transport_sendPacketBuffer(mysock, buf, len);
			if(rc != -1)
			{
				USART_OUT(USART1, "PINGREQ\r\n");
			}
		break;
		
		case PINGRESP:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
			if(rc == 1)
			{
				gprs_sleep_status = gprs_sleep();
				if(gprs_sleep_status == 1)
				{
					USART_OUT(USART1, "keep_alive gprs_sleep ok\r\n");	
				}
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


int mqtt_client1(list_node **list_recv, list_node **list_send, uint8_t msg_tpye)
{
	int size = 0;
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
	int mqtt_msg_tpye = 0; 
	int gprs_sleep_status = 0;
	int gprs_wakeup_status = 0;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s *msg = NULL;
	
	memset(buf, 0, sizeof(buf));
	
	if(msg_tpye != 0)
	{
		mqtt_stauts = msg_tpye;
	}
	else
	{
		mqtt_stauts = MQTTPacket_read(buf, buflen, transport_getdata);
	}
	switch(mqtt_stauts)
	{
		case CONNECT:
			
		break;
		
		case CONNACK:
			
		break;
		
		case PUBLISH:
			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
			&payload_in, &payloadlen_in, buf, buflen);		//subscriber publish
			if(rc == 1)
			{
				if(qos == 2)
				{	
					USART_OUT(USART1, "subscriber PUBLISH=%d\r\n", msgid);
					memset(&mqtt_msg, 0, sizeof(mqtt_msg));
					
					memcpy(mqtt_msg.topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
					memcpy(mqtt_msg.payload, payload_in, payloadlen_in);
//					memcpy(mqtt_msg.topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
//					memcpy(mqtt_msg.payload, payload_in, payloadlen_in);
					mqtt_msg.payloadlen = payloadlen_in;
					mqtt_msg.retained = retained;
					mqtt_msg.dup = dup;
					mqtt_msg.qos = qos;
					mqtt_msg.msg_id = msgid;
					mqtt_msg.status = PUBREC;
									
					size = list_size(*list_recv);
					if(size <= 5)
					{
						msg = list_get_addr_by_msgid(*list_send ,msgid);
						if(msg->msg_id != msgid)
						{
							list_insert_last(list_recv, mqtt_msg);
							list_travese(list_recv);
						}
						else
						{
							USART_OUT(USART1, "subscriber msgid repeat =%d\r\n", msgid);
						}
					}
					else
					{
						msg = list_find_min_val(list_recv);
						list_de_by_elem(list_recv, msg->msg_id);
						USART_OUT(USART1, "subscriber msgid delete =%d\r\n", msg->msg_id);
					}
					USART_OUT(USART1, "list_size=%d\r\n", size);
					////
					memset(buf, 0, sizeof(buf));
					len = MQTTSerialize_pubrec(buf, buflen, msgid);		//subscriber pubrec
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{	
						USART_OUT(USART1, "subscriber PUBREC=%d\r\n", msgid);
					}	
				}	
				else if(qos == 1)
				{
			
					
					memset(buf, 0, sizeof(buf));
					len = MQTTSerialize_puback(buf, buflen, msgid);
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
						timer_is_timeout_1ms(timer_heartbeat, 0);
	//					gprs_sleep_status = gprs_sleep();
	//					if(gprs_sleep_status == 1)
	//					{
	//						USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
	//					}
						mqtt_stauts = 0;

						USART_OUT(USART1, "subscriber PUBACK=%d\r\n", msgid);
					}
				}
			}						
		break;
	
		case PUBACK:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
				timer_is_timeout_1ms(timer_heartbeat, 0);
				USART_OUT(USART1, " PUBCOMP=%d\r\n", msgid);	
				
//				gprs_sleep_status = gprs_sleep();
//				if(gprs_sleep_status == 1)
//				{
//					USART_OUT(USART1, "publist gprs_sleep ok\r\n");	
//				}
				//消息id
//				mqtt_publist_msgid++;
//				if(mqtt_publist_msgid >= 65535)
//				{
//					mqtt_publist_msgid = 0;
//				}
			}	
			
		break;
		
		case PUBREC:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);	//publisher	pubrec
			if(rc == 1)
			{
				USART_OUT(USART1, "publisher PUBREC=%d\r\n", msgid);
				
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubrel(buf, buflen, 0, msgid);	//publisher  pubrel
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					list_modify_elem(list_send, msgid, PUBREL);
//					list_travese(list_send);
					
					USART_OUT(USART1, "publisher PUBREL=%d\r\n", msgid);
				}	
			}	
		break;
			
		case PUBREL:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);	//subscriber pubrel
			if(rc == 1)
			{		
				USART_OUT(USART1, "subscriber PUBREL=%d\r\n", msgid);	
				
				////
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubcomp(buf, buflen, msgid);	
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{	
					list_modify_elem(list_recv, msgid, PUBCOMP);
//					list_travese(list_recv);
					
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					timer_is_timeout_1ms(timer_heartbeat, 0);
					
					gprs_sleep_status = gprs_sleep();
					if(gprs_sleep_status == 1)
					{
						USART_OUT(USART1, "subscriber gprs_sleep ok\r\n");	
					}
					
					USART_OUT(USART1, "subscriber PUBCOMP=%d\r\n", msgid);
				}	
			}
		break;
		
		case PUBCOMP:
			rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);		//publisher 
			if(rc == 1)
			{		
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
				timer_is_timeout_1ms(timer_heartbeat, 0);
				USART_OUT(USART1, "publisher PUBCOMP=%d\r\n", msgid);
				
				
				msg = list_get_addr_by_msgid(*list_send ,msgid);
				if(msg->msg_id == msgid)
				{
					list_de_by_elem(list_send, msg->msg_id);
					list_travese(list_send);
				}
							
				gprs_sleep_status = gprs_sleep();
				if(gprs_sleep_status == 1)
				{
					USART_OUT(USART1, "publisher gprs_sleep ok\r\n");	
				}
				//消息id
				mqtt_publist_msgid++;
				if(mqtt_publist_msgid >= 65535)
				{
					mqtt_publist_msgid = 0;
				}
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
			len = MQTTSerialize_pingreq(buf, buflen);
			rc = transport_sendPacketBuffer(mysock, buf, len);
			if(rc != -1)
			{
				USART_OUT(USART1, "PINGREQ\r\n");
			}
		break;
		
		case PINGRESP:
			rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
			if(rc == 1)
			{
				gprs_sleep_status = gprs_sleep();
				if(gprs_sleep_status == 1)
				{
					USART_OUT(USART1, "keep_alive gprs_sleep ok\r\n");	
				}
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





int mqtt_subscribe11(unsigned char* topic, unsigned char *payload, int *payloadlen)
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
				USART_OUT(USART1, "mqtt_msg_tpye=%d\n", mqtt_msg_tpye);	
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
						subscribe_status = PUBREC;	
						USART_OUT(USART1, "Topic=%s======qos=%d\r\n", topic, qos);
						USART_OUT(USART1, "PUBLISH=%d\r\n", msgid);
						
						
					}	
					else if(qos == 1)
					{
						memcpy(topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
						memcpy(payload, payload_in, *payloadlen);			
//						subscribe_status = PUBACK;
						USART_OUT(USART1, "Topic=%s======qos=%d\r\n", topic, qos);
						USART_OUT(USART1, "PUBLISH=%d\r\n", msgid);
						
				
					}
				}		
			break;
				
			case PUBACK:	//4
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_puback(buf, buflen, msgid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					timer_is_timeout_1ms(timer_heartbeat, 0);
//					gprs_sleep_status = gprs_sleep();
//					if(gprs_sleep_status == 1)
//					{
//						USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
//					}
					ret = 1;
					status = 1;

					USART_OUT(USART1, "PUBACK=%d\r\n", msgid);
				}
			break;
				
			case PUBREC:	//5		
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubrec(buf, buflen, msgid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					subscribe_status = 0;	
					
					USART_OUT(USART1, "PUBREC=%d\r\n", msgid);
				}
			break;
				
			case PUBREL:	//6	
				rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
				if(rc == 1)
				{		
					ret = 0;
//					subscribe_status = PUBCOMP;
					USART_OUT(USART1, "PUBREL=%d\r\n", msgid);	
					
					memset(buf, 0, sizeof(buf));
					len = MQTTSerialize_pubcomp(buf, buflen, msgid);
					rc = transport_sendPacketBuffer(mysock, buf, len);
					if(rc != -1)
					{					
						timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
						timer_is_timeout_1ms(timer_heartbeat, 0);
	//					gprs_sleep_status = gprs_sleep();
	//					if(gprs_sleep_status == 1)
	//					{
	//						USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
	//					}

						ret = 1;
						status = 1;
						USART_OUT(USART1, "PUBCOMP=%d\r\n", msgid);
					}
					
				}
			break;

			case PUBCOMP:	//7
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubcomp(buf, buflen, msgid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{					
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					timer_is_timeout_1ms(timer_heartbeat, 0);
//					gprs_sleep_status = gprs_sleep();
//					if(gprs_sleep_status == 1)
//					{
//						USART_OUT(USART1, "subscribe gprs_sleep ok\r\n");	
//					}

					ret = 1;
					status = 1;
					USART_OUT(USART1, "PUBCOMP=%d\r\n", msgid);
				}
			break;
				
			default:
			break;	
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


int mqtt_publist11(unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid)
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
		
		switch(publist_status)
		{
			case PUBLISH:
				gprs_wakeup_status = gprs_wakeup(0);
//				if(gprs_wakeup_status == 1)
//				{
//					USART_OUT(USART1, "gprs_wakeup ok\r\n");
//				}
				timer_is_timeout_1ms(timer_mqtt_publist_timeout, 0);
				topicString.cstring = topic;				
//				strcpy(topicString.cstring, "test");	//死机
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_publish((unsigned char *)buf , buflen, 0, qos, 0, packetid, topicString, (unsigned char*)payload, payload_len);
				rc = transport_sendPacketBuffer(mysock, buf, len);	
				if(rc != -1)
				{
					publist_status = 0;
					USART_OUT(USART1, "PUBLISH\r\n");
					USART_OUT(USART1, "PUBLISH packetid=%d\r\n", packetid);
				}				
			break;
				
			case PUBREC:	
				rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);
				if(rc == 1)
				{
					if(packetid == msgid)
					{
						publist_status = PUBREL;
						USART_OUT(USART1, "PUBREC\r\n");	
					}
				}										
			break;
				
			case PUBREL:
				memset(buf, 0, sizeof(buf));
				len = MQTTSerialize_pubrel(buf, buflen, 0, packetid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				if(rc != -1)
				{
					publist_status = 0;
					USART_OUT(USART1, "PUBREL\r\n");
				}
				
			break;

			case PUBCOMP:
				rc = MQTTDeserialize_ack(&type, &dup, &msgid, buf, buflen);
				if(packetid == msgid)
				{		
					ret = 1;
					status = 1;
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					timer_is_timeout_1ms(timer_heartbeat, 0);
					USART_OUT(USART1, "PUBCOMP\r\n");	
//					gprs_sleep_status = gprs_sleep();
//					if(gprs_sleep_status == 1)
//					{
//						USART_OUT(USART1, "publist gprs_sleep ok\r\n");	
//					}
					//消息id
					mqtt_publist_msgid++;
					if(mqtt_publist_msgid >= 65535)
					{
						mqtt_publist_msgid = 0;
					}
				}	
			break;
				
			default:
			break;	
		}

		if(timer_is_timeout_1ms(timer_mqtt_resend, 5000) == 0)
		{
				
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_publist_timeout, 5000) == 0)
		{
			publist_status = PUBLISH;
			USART_OUT(USART1, "=============================mqtt_publist_err_cnt\r\n");
			mqtt_publist_err_cnt++;
			if(mqtt_publist_err_cnt > 5)
			{
				ret = 1;
				status = 0;
				USART_OUT(USART1, "=============================mqtt_publist_err_cnt error\r\n");
			}

		}
	}
	
	return status;
}






