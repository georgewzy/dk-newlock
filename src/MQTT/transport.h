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
#ifndef __TRANSPORT_H_
#define __TRANSPORT_H_
#include "bsp.h"
#include "MQTTPacket.h"




typedef struct
{
	MQTTHeader	m_header;
	int	msg_len;
	int topic_len;
	MQTTString m_string;
	unsigned short msg_id;
	char *payload;
}mqtt_protocol;




typedef struct 
{
	char payload[100];
	char topic[50];
//	char *payload;
//	char *topic;
	int payloadlen;
	int topiclen;
	unsigned short msg_id;
	unsigned char retained;
	unsigned char dup;
	int qos;
	int status;
} mqtt_msg_s;



typedef struct Node
{
	mqtt_msg_s msg;
	struct Node *next;
	
}list_node, *p_node;



#define MQTT_MSG_DEFAULT() {{0},0,0}







int mqtt_keep_alive_test(void);

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);

int mqtt_publish_qos0(unsigned char* topic, unsigned char* payload, int payload_len);
int mqtt_publish_qos2(list_node **list, unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid);

int mqtt_connect(MQTTPacket_connectData *pdata);
int mqtt_disconnect(void);
int mqtt_publish(unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid);
int mqtt_subscribe(unsigned char* topic, unsigned char *payload, int *payloadlen);
int mqtt_subscribe_topic(unsigned char* topic, int req_qos, unsigned short packetid);
int mqtt_keep_alive(uint32_t ms);
int mqtt_client(list_node **list_recv, list_node **list_send, uint8_t msg_tpye);


#endif
