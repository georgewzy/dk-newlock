/*************************************************
  Copyright (C), 1988-1999,  Tech. Co., Ltd.
  File name:      
  Author:       Version:        Date:
  Description:   
  Others:         
  Function List:  
    1. ....
  History:         
                   
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef __GPRS_H_
#define __GPRS_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stdbool.h>
#include  "bsp.h"
#include "stm32l1xx.h"
#include "transport.h"


typedef struct
{
	uint8_t *server_ip;	
	uint32_t server_port;
}GPRS_CONFIG;




#define GPRS_CONFIG_INIT 	{NULL, 0}






void gprs_gpio_init(void);
void gprs_power_on(void);
uint8_t *gprs_send_at(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout);
uint8_t* gprs_send_at1(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout);
uint8_t* gprs_send_at2(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout);
uint8_t* gprs_send_at3(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout);
int gprs_send_data(uint8_t *data, uint16_t data_len, uint8_t *ack, uint16_t waittime);
uint8_t gprs_init(void);
void gprs_init_task(list_node ** list, GPRS_CONFIG *gprs_info, MQTTPacket_connectData *mqtt_data);
int gprs_sleep(void);
int gprs_wakeup(uint8_t mode);
int gprs_wakeup_dtr(void);

#endif

