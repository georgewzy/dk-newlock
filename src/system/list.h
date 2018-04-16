


#ifndef __LIST_H_
#define __LIST_H_
#include <stdbool.h>
#include "bsp.h"
#include "transport.h"


typedef struct Node
{
	mqtt_msg_s msg;
//	char *data;
	struct Node *next;
	
}list_node, *p_node;




void list_travese(list_node *p_head);
void list_insert_last(list_node **p_head, mqtt_msg_s *m_data);
void list_de_last(list_node **p_head);
void list_test(void);

#endif
