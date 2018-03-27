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
uint8_t *gprs_send_at(u8 *cmd, u8 *ack, u16 waittime, u16 timeout);
int gprs_send_data(u8 *data, u16 data_len, u8 *ack, u16 waittime);
uint8_t gprs_init(void);
void gprs_init_task(GPRS_CONFIG *gprs_info, MQTTPacket_connectData *mqtt_data);
int gprs_sleep(void);
int gprs_wakeup(uint8_t mode);


#endif

