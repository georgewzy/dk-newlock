


#ifndef __LIST_H_
#define __LIST_H_
#include <stdbool.h>
#include "bsp.h"
#include "transport.h"




//typedef struct Node
//{
//	mqtt_msg_s msg;
//	struct Node *next;
//	
//}list_node, *p_node;





int list_size(list_node *p_head);
int list_is_empty(list_node **p_head);
void list_travese(list_node **p_head);
void list_insert_last(list_node **p_head, mqtt_msg_s m_data);
void list_de_last(list_node **p_head);
void list_de_by_elem(list_node **p_head, int msg_id);
mqtt_msg_s *list_get_addr_by_msgid(list_node *p_head, int msg_id);
mqtt_msg_s *list_get_addr_by_status(list_node *p_head, int status);
void list_test(list_node **list);
void list_test2(list_node **list);


#endif
