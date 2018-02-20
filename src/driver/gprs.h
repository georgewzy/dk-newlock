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
#include  "bsp.h"
#include <stm32l1xx.h>
#include <stdbool.h>













#define GPRS_STATUS_0						0
#define GPRS_STATUS_1						1
#define GPRS_STATUS_2						2
#define GPRS_STATUS_3						3
#define GPRS_STATUS_4						4
#define GPRS_STATUS_5						5
#define GPRS_STATUS_6						6
#define GPRS_STATUS_7						7
#define GPRS_STATUS_8						8
#define GPRS_STATUS_9						9
#define GPRS_STATUS_10						10
#define GPRS_STATUS_11						11



void gprs_power_on(void);



uint8_t *gprs_send_at(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout);
uint8_t send_at_cmd(uint8_t *cmd, uint8_t *str, uint32_t timeout);
uint8_t gprs_init(void);
void gprs_task_create(void);
void gprs_config(void);

static void gprs_init_task_fun(void *p_arg);
static void gprs_recv_task_fun(void *p_arg);

#endif

