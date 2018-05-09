

#ifndef __LOCK_H_
#define __LOCK_H_
#include "bsp.h"
#include "transport.h"




void lock_gpio_init(void);
void lock_find_bell(void);
void lock_shake_alarm(void);
void lock_self_checking(void);
void lock_hand_close(void);
void lock_self_checking(void);
void lock_open_deal(void);
void lock_close_deal(void);
void lock_hand_close(void);
void lock_close_deal_1(list_node **list);
void lock_open_deal_1(list_node **list);


#endif
