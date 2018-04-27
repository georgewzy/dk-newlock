


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




void list_clear(list_node **p_head);
int list_size(list_node *p_head);
int list_is_empty(list_node **p_head);
void list_travese(list_node **p_head);
void list_send_travese(list_node **p_head);
void list_insert_last(list_node **p_head, mqtt_msg_s m_data);
void list_de_last(list_node **p_head);
void list_de_by_msgid(list_node **p_head, int msg_id);
mqtt_msg_s *list_get_addr_by_msgid(list_node *p_head, int msg_id);
mqtt_msg_s *list_get_addr_by_status(list_node *p_head, int status);
mqtt_msg_s *list_find_min_val(list_node **p_head);
void list_test(list_node **list);
void list_test2(list_node **list);
int list_modify_status(list_node **p_node, int msg_id, int status);
int list_modify_timer_id(list_node **p_node, int msg_id, int timer_id);






#endif
