



#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include  "bsp.h"
#include "stm32l1xx.h"
#include "list.h"


typedef struct
{
	uint16_t pdata_len;
	
	uint8_t *topic;
	uint8_t *pdata;

} mqtt_telegram_t;











void protocol_analyze1(list_node **list);
void dev_to_srv_batt_voltage(list_node **list, uint32_t ms);
void dev_to_srv_lock_status(list_node **list);
void dev_first_power_on(list_node **list);
void heartbeat(list_node **list, uint32_t ms);
void heartbeat_qos0(uint32_t ms);
void mqtt_keep_alive(list_node *list_recv, list_node *list_send, int mqtt_stauts);

#endif
