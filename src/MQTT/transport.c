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



extern usart_buff_t usart1_rx_buff;
extern usart_buff_t usart2_rx_buff;
extern usart_buff_t mqtt_buff;
extern u8 send_buff[100];


u8 subscribe_status = PUBLISH;

int mqtt_buff_len = 0;

u32 packet_id = 0;
uint16_t mqtt_publist_msgid = 0;				//发布消息id
uint16_t mqtt_subscribe_msgid = 0;				//订阅消息id

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
//static int mysock = INVALID_SOCKET;


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	u8 *ret;
	int len = 0;
	u8 cmd[30] = {0};
//	u8 end_char[2] = {0};
//	end_char[0] = 0x1A;//结束字符
//	end_char[1] = '\0';
	
	
	memset(send_buff, 0, sizeof(send_buff));
	memset(&mqtt_buff, 0, sizeof(usart_buff_t));
	mqtt_buff_len = 0;
	sprintf((char *)cmd, "AT+CIPSEND=%d,1\r\n", buflen);
	ret = gprs_send_at(cmd, ">", 20, 100);
	if(ret != NULL)
	{
		memcpy((char *)send_buff, buf, buflen);
//		memcpy((char *)send_buff+buflen, (char*)end_char, sizeof(end_char));
		usart_send(USART2, send_buff, buflen);	
		
		mqtt_publist_msgid++;
		if(mqtt_publist_msgid >= 65535)
		{
			mqtt_publist_msgid = 0;
		}
		
		rc = buflen;
	}
	
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	if(mqtt_buff.index > 0)
	{
		memcpy(buf, &mqtt_buff.pdata[mqtt_buff_len], count);
//		usart_send(USART1, mqtt_buff.pdata, mqtt_buff.index);
//		usart_send(USART1, &mqtt_buff->pdata[mqtt_buff_len], count);
		mqtt_buff_len += count;
		return count;
	}
	
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
	u8 connect_status = CONNECT;
	
	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{ 
		usart2_recv_data();

		switch(connect_status)
		{
			case CONNECT:
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
					}
				}
			break;
			
			default:
			break;	
		}	
		
		if(timer_is_timeout_1ms(timer_mqtt_timeout, 2000) == 0)
		{
			ret = 1;
			status = 0;
		}
		
	}
	
	return status;
}




int mqtt_publist(unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid)
{
	int status = 0;
	u8 ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200];
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;
	u8 publist_status = PUBLISH;
	
	
	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{
		usart2_recv_data();
		switch(publist_status)
		{
			case PUBLISH:
				topicString.cstring = topic;				
//				strcpy(topicString.cstring, "test");
				len = MQTTSerialize_publish((unsigned char *)buf , buflen, 0, qos, 0, packetid, topicString, (unsigned char *)payload, payload_len);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				publist_status = PUBREC;
				USART_OUT(USART1, "PUBLISH\r\n");
			break;
				
			case PUBREC:
				if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBREC)
				{
					publist_status = PUBREL;
					USART_OUT(USART1, "PUBREC\r\n");
				}	
			break;
				
			case PUBREL:
				len = MQTTSerialize_pubrel(buf, buflen, 0, packetid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				publist_status = PUBCOMP;
				USART_OUT(USART1, "PUBREL\r\n");
			break;

			case PUBCOMP:
				if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBCOMP)
				{
					ret = 1;
					status = 1;
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					USART_OUT(USART1, "PUBCOMP\r\n");
					
				}	
			break;
				
			default:
			break;	
		}

		if(timer_is_timeout_1ms(timer_mqtt_timeout, 3000) == 0)
		{
			ret = 1;
			status = 0;
		}
	}
	
	return status;
}

int mqtt_subscribe(unsigned char* topic, unsigned char *payload, int *payloadlen)
{
	int status = 0;
	u8 ret = 0;
	int rc = 0;
	int len = 0;
	char buf[200] = {0};
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTString topicString = MQTTString_initializer;

	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{
		switch(subscribe_status)
		{
			case PUBLISH:
				ret = 1;
				if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
				{
					unsigned char dup;
					int qos;
					unsigned char retained;
					int rc;
					unsigned char* payload_in;
					MQTTString receivedTopic;
					
					rc = MQTTDeserialize_publish(&dup, &qos, &retained, &mqtt_subscribe_msgid, &receivedTopic,
					&payload_in, payloadlen, buf, buflen);
					
					USART_OUT(USART1, "message arrived  payloadlen=%d=payload_in=%s=mqtt_subscribe_msgid=%d\r\n", *payloadlen, payload_in, mqtt_subscribe_msgid);
					memcpy(topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
					memcpy(payload, payload_in, *payloadlen);			
					subscribe_status = PUBREC;
					ret = 0;
					USART_OUT(USART1, "Topic=%s\n", topic);
					USART_OUT(USART1, "PUBLISH\r\n");
				}
			break;
				
			case PUBREC:
				len = MQTTSerialize_pubrec(buf, buflen, mqtt_subscribe_msgid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				subscribe_status = PUBREL;	
				USART_OUT(USART1, "PUBREL\r\n");
			break;
				
			case PUBREL:
				usart2_recv_data();
				if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBREL)
				{
					unsigned char type = 0;
					unsigned short msgid;
					int rc;
					rc = MQTTDeserialize_ack(&type, 0, &msgid, buf, buflen);
					if(type == PUBREL && msgid == mqtt_subscribe_msgid)
					{
						USART_OUT(USART1, "message ack type=%d==msgid=%d\n", type, msgid);
						subscribe_status = PUBCOMP;
					}	
				}				
			break;

			case PUBCOMP:
				len = MQTTSerialize_pubcomp(buf, buflen, mqtt_subscribe_msgid);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				
				timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
				
				subscribe_status = PUBLISH;
				ret = 1;
				status = 1;
				USART_OUT(USART1, "PUBCOMP\r\n");
			break;
				
			default:
			break;	
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_timeout, 2000) == 0)
		{
			ret = 1;
			status = 0;
		}
		
	}

	return status;
}



int mqtt_subscribe_topic(unsigned char* topic, int req_qos, unsigned short packetid)
{
	int status = 0;
	int ret = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int len = 0;
	int rc = 0;
	int mysock = 0;
	u8 subscribe_status = SUBSCRIBE;
	MQTTString topicString = MQTTString_initializer;
	
	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{
		usart2_recv_data();
		
		switch(subscribe_status)
		{
			case SUBSCRIBE:
				/* subscribe */
				topicString.cstring = topic;
				len = MQTTSerialize_subscribe(buf, buflen, 0, packetid, 1, &topicString, &req_qos);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				subscribe_status = SUBACK;
				USART_OUT(USART1, "SUBSCRIBE\r\n");
			break;
			
			case SUBACK:
				if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
				{
					unsigned short submsgid;
					int subcount;
					int granted_qos;

					rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
					USART_OUT(USART1, "granted qos=%d submsgid=%d\n", granted_qos, submsgid);	
					ret = 1;
					status = 1;
					timer_is_timeout_1ms(timer_mqtt_keep_alive, 0);
					
					USART_OUT(USART1, "SUBACK\r\n");
				}
			break;
				
			default:
			break;	
			
		}
		
		if(timer_is_timeout_1ms(timer_mqtt_timeout, 2000) == 0)
		{
			ret = 1;
			status = 0;
		}
		
	}
	
	return status;
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
	u8 keep_alive_status = PINGREQ;
	

	timer_is_timeout_1ms(timer_mqtt_timeout, 0);
	while(!ret)
	{
		usart2_recv_data();	
		
		switch(keep_alive_status)
		{
			case PINGREQ:	
				len = MQTTSerialize_pingreq(buf, buflen);
				rc = transport_sendPacketBuffer(mysock, buf, len);
				keep_alive_status = PINGRESP;
			break;
			
			case PINGRESP:
				if(MQTTPacket_read(buf, buflen, transport_getdata) == PINGRESP)
				{
					ret = 1;
					status = 1;
					USART_OUT(USART1, " mqtt_keep_alive\r\n");
				}		
			break;
			
			default:
			break;					
		}

		if(timer_is_timeout_1ms(timer_mqtt_timeout, 3000) == 0)
		{
			ret = 1;
			status = 0;
		}
	}
	
	return status;
}

















