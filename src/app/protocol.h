



#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include  "bsp.h"
#include "stm32l1xx.h"


typedef struct
{
	uint16_t pdata_len;
	
	uint8_t *topic;
	uint8_t *pdata;

} mqtt_telegram_t;











void protocol_analyze(void);

#endif
