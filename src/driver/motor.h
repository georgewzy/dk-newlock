


#ifndef __MOTOR_H_
#define __MOTOR_H_

#include  "bsp.h"
#include <stm32l1xx.h>






void lock_open(void);
void lock_close(void);
void lock_stop(void);
uint8_t lock_on_status_get(void);
uint8_t lock_off_status_get(void);

#endif