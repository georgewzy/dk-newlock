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

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);
<<<<<<< HEAD



int mqtt_connect(void);
int mqtt_publist(unsigned char* topic, unsigned char* payload, int payload_len, int qos, unsigned short packetid);
void mqtt_subscribe(unsigned char* topic, unsigned char* payload, int payloadlen);
int mqtt_subscribe_msg(unsigned char* topic, int req_qos, unsigned short packetid);
int mqtt_keep_alive(void);

#endif
=======
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);
>>>>>>> parent of 59d2934... c
