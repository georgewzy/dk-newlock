

#include <stdlib.h>
#include <string.h>		
#include <stdio.h>
#include "list.h"
#include "usart.h"

extern list_node *mqtt_recv_list;
extern list_node *mqtt_send_list;




void list_init(list_node **p_head)
{
	*p_head = NULL;
	
}

list_node *list_node_init2(mqtt_msg_s m_data)
{
	list_node *p_node = NULL;
	
//	p_node->msg.buff = m_data->buff;
//	p_node->msg.buff_len = m_data->buff_len;
//	p_node->msg.status = m_data->status;
	
	p_node->msg = m_data;
	p_node->next = NULL;	

	return p_node;
}

list_node *list_node_init1(char *data)
{
	list_node *p_node = NULL;
	
//	p_node->data = data;
	p_node->next = NULL;	

	return p_node;
}


list_node *list_node_init(char *data)
{
	list_node *p_node;
	
	p_node = (list_node *)malloc(sizeof(list_node));
	if(p_node == NULL)
	{
		return NULL;
	}
	
//	p_node->data = data;
	p_node->next = NULL;	

	return p_node;
}




list_node *list_creat(list_node *p_head)
{
	list_node *p1;
	list_node *p2;

	p1 = p2 = (list_node *)malloc(sizeof(list_node));
	if(p1 == NULL || p2 == NULL)
	{
		return NULL;
	}
	
	
	
}


void list_travese(list_node *p_head)
{
	while(NULL != p_head)
	{
		USART_OUT(USART1, "p_head->msg.buff=%s\r\n", p_head->msg.buff);	
		USART_OUT(USART1, "p_head->msg.msg_id=%d\r\n", p_head->msg.msg_id);
//		usart_send_data(USART1, p_head->msg.buff, sizeof(p_head->msg.buff)-1);
		p_head = p_head->next;
	}
	
}



void list_clear(list_node *p_head)
{
	list_node *p_next;
	
	if(p_head == NULL)
	{
		return;
	}
	
	while(p_head->next != NULL)
	{
		p_next = p_head->next;
		free(p_head);
		p_head = p_next;
	}
}




int list_size(list_node *p_head)
{
	int size = 0;
	
	while(p_head != NULL)
	{
		size++;
		p_head = p_head->next;
	}
	
	return size;
}

int list_is_full(list_node *p_head)
{

}

int list_is_empty(list_node *p_head)
{
	if(p_head == NULL)
	{
		return 1;
	}
		
	return 0;
}




void list_insert_last(list_node **p_head, mqtt_msg_s *m_data)
{
	list_node *p_node;
	list_node *p_tmp = *p_head;	//定义一个临时链表用来存放第一个节点
	
	p_node = (list_node *)malloc(sizeof(list_node));
	
	if(NULL == *p_head)
	{	
		p_node->msg = *m_data;
		p_node->next = NULL;
		*p_head = p_node;			
	}
	else
	{
		while(p_tmp->next != NULL)
		{
			p_tmp = p_tmp->next;
		}
		
		p_node->msg = *m_data;
		p_node->next = NULL;
		p_tmp->next = p_node;
	}
	
//	free(p_node);	//此处不能加free 愿意未知
}


void list_de_last(list_node **p_head)
{
	list_node *tmp_node = NULL;
	list_node *cur_node = NULL;
	
		
	if(*p_head == NULL)
	{
		return;
	}
	else if(NULL == (*p_head)->next)
	{
		tmp_node = *p_head;
		*p_head = NULL;
	}
	else
	{
		cur_node = *p_head;
		while(cur_node->next->next)
		{
//			USART_OUT(USART1, "delete cur_node->data=%s\r\n", cur_node->data);
			cur_node = cur_node->next;	
		}
		
		cur_node->next = NULL;
	}	
}
mqtt_msg_s *list_get_elem(list_node *p_head, int msg_id )
{

}

mqtt_msg_s *list_get_elem_addr(list_node *p_head, int msg_id)
{
	if(NULL == p_head)
	{
		return NULL;
	}
	
	while((p_head->msg.msg_id  != msg_id) && (NULL != p_head->next))//判断是否到链表的尾部，以及存在要找的元素
	{
		p_head = p_head->next;
	}
	
	if((p_head->msg.msg_id != msg_id) && (p_head != NULL))//未找到
	{
		return NULL;
	}
	
	if(p_head->msg.msg_id == msg_id)
	{
		USART_OUT(USART1, "list_get_elem_addr=%d=%d\r\n", msg_id, &(p_head->msg));
	}
	
	return &(p_head->msg);
}


int list_modify_elem(list_node *p_node, int msg_id, int status)
{
	list_node *p_head;
	p_head = p_node;
	
	if(NULL == p_head)
	{
		return NULL;
	}
	
	while((p_head->msg.msg_id  != msg_id) && (NULL != p_head->next))//判断是否到链表的尾部，以及存在要找的元素
	{
		p_head = p_head->next;
	}
	
	if((p_head->msg.msg_id != msg_id) && (p_head != NULL))//未找到
	{
		return NULL;
	}
	
	if(p_head->msg.msg_id == msg_id)
	{
		p_head->msg.status = status;
		USART_OUT(USART1, "list_get_elem_addr=%d=%d\r\n", msg_id, &(p_head->msg));
	}
	
	return 1;
}


void list_test(void)
{
	int size = 0;
	list_node *list = NULL;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s mqtt_msg1;
	mqtt_msg_s mqtt_msg2;
	mqtt_msg_s mqtt_msg3;
	mqtt_msg_s mqtt_msg5;
	
	char a[55]="aaa";
	char b[55]="bbb";
	char c[55]="ccc";
	char d[55]="ddd";
	char e[55]="eee";
	char f[55]="fff";
	char g[55]="ggg";
	
	mqtt_msg1.buff = b;
	mqtt_msg1.buff_len = 3;
	mqtt_msg1.msg_id = 1;
	mqtt_msg1.status = PUBLISH;
	
	mqtt_msg2.buff = c;
	mqtt_msg2.buff_len = 3;
	mqtt_msg2.msg_id = 2;
	mqtt_msg2.status = PUBLISH;
	
	mqtt_msg3.buff = d;
	mqtt_msg3.buff_len = 3;
	mqtt_msg3.msg_id = 3;
	mqtt_msg3.status = PUBLISH;
	
	mqtt_msg.buff = a;
	mqtt_msg.buff_len = 3;
	mqtt_msg.msg_id = 4;
	mqtt_msg.status = PUBLISH;
	
	usart_send_data(USART1, mqtt_msg.buff, 3);
	
	size = list_size(mqtt_send_list);
	
	list_travese(mqtt_send_list);
	list_insert_last(&mqtt_send_list, &mqtt_msg);
	list_travese(mqtt_send_list);
	list_insert_last(&mqtt_send_list, &mqtt_msg1);
	list_insert_last(&mqtt_send_list, &mqtt_msg2);
	size = list_size(mqtt_send_list);	
	list_travese(mqtt_send_list);
	
	list_get_elem_addr(mqtt_send_list, 1);
	USART_OUT(USART1, "mqtt_send_list->msg.buff=%s\r\n", mqtt_send_list->msg.buff);	
	USART_OUT(USART1, "mqtt_send_list->msg.msg_id=%d\r\n", mqtt_send_list->msg.msg_id);
		
	list_de_last(&mqtt_send_list);
	list_travese(mqtt_send_list);
	size = list_size(mqtt_send_list);
	
	
	list_insert_last(&mqtt_send_list, &mqtt_msg3);
	list_travese(mqtt_send_list);
	
	
	
//	list_travese(list);
//	list_insert_last(&list, a);
//	list_travese(list);
//	list_insert_last(&list, b);
//	list_travese(list);
//	list_de_last(&list);
//	list_travese(list);
//	list_insert_last(&list, c);
//	list_travese(list);
//	list_insert_last(&list, d);
//	list_travese(list);
	
	
	size = list_size(mqtt_send_list);
	
	USART_OUT(USART1, "list_size=%d\r\n", size);
	
}







