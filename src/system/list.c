

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


void list_travese(list_node **p_head)
{
	int size = 0;
	list_node *tmp = *p_head;
	if(NULL == tmp)
	{
		USART_OUT(USART1, "list_travese list is NULL =%d\r\n", p_head);
	}
	else
	{
		USART_OUT(USART1, "list_travese_p_head =%d\r\n", p_head);
		USART_OUT(USART1, "list_travese_AAp_head =%d\r\n", *p_head);
		while(NULL != tmp)
		{		
			size++;
			USART_OUT(USART1, "list_travese_tmp=%d\r\n", tmp);
			USART_OUT(USART1, "p_head->msg.payload=%s\r\n", tmp->msg.payload);
			USART_OUT(USART1, "p_head->msg.topic=%s\r\n", tmp->msg.topic);
			USART_OUT(USART1, "p_head->msg.payloadlen=%d\r\n", tmp->msg.payloadlen);
			USART_OUT(USART1, "p_head->msg.msg_id=%d\r\n", tmp->msg.msg_id);
			USART_OUT(USART1, "p_head->msg.status=%d\r\n", tmp->msg.status);
			USART_OUT(USART1, "p_head->next=%d\r\n", tmp->next);
			tmp = tmp->next;
		}
	}
	
	USART_OUT(USART1, "list_travese_size=====%d\r\n", size);
}

void list_send_travese(list_node **p_head)
{
	int size = 0;
	list_node *tmp = *p_head;
	
	USART_OUT(USART1, "list_send_travese_p_head1 =%d\r\n", p_head);
	USART_OUT(USART1, "list_send_travese_AAp_head1 =%d\r\n", *p_head);
	while(NULL != tmp)
	{
		size++;
		USART_OUT(USART1, "send_list_travese_tmp =%d\r\n", tmp);
		USART_OUT(USART1, "send_p_head->msg.payload=%s\r\n", tmp->msg.payload);
		USART_OUT(USART1, "send_p_head->msg.topic=%s\r\n", tmp->msg.topic);
		USART_OUT(USART1, "send_p_head->msg.payloadlen=%d\r\n", tmp->msg.payloadlen);
		USART_OUT(USART1, "send_p_head->msg.msg_id=%d\r\n", tmp->msg.msg_id);
		USART_OUT(USART1, "send_p_head->msg.status=%d\r\n", tmp->msg.status);
		USART_OUT(USART1, "send_p_head->next=%d\r\n", tmp->next);
		tmp = tmp->next;
	}
	
	USART_OUT(USART1, "list_send_travese_size=====%d\r\n", size);
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
	list_node *tmp = p_head;
	while(tmp != NULL)
	{
		size++;
		tmp = tmp->next;
	}
	
	return size;
}

int list_is_full(list_node *p_head)
{

}

int list_is_empty(list_node **p_head)
{
	if((*p_head) == NULL)
	{
		return -1;
	}
		
	return 1;
}


void list_insert_last(list_node **p_head, mqtt_msg_s m_data)
{
	int sizeaa = 0;
	list_node *p_insert = NULL;
	list_node *p_tmp = *p_head;	//定义一个临时链表用来存放第一个节点
		

	p_insert = (list_node *)malloc(sizeof(list_node));
	if(p_insert == NULL)
	{
		USART_OUT(USART1, "p_insert_return\r\n");
		return;
	}
	memset(p_insert, 0, sizeof(list_node));
	p_insert->msg = m_data;
	p_insert->next = NULL;

	if(NULL == *p_head)
	{			
		*p_head = p_insert;	
//		p_insert->next = NULL;	//20180423晚上添加
	}
	else
	{	
		while(p_tmp->next != NULL)
		{
			p_tmp = p_tmp->next;
		}
		p_tmp->next = p_insert;
	}
	
//	free(p_insert);	//此处不能加free 原因未知
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
			cur_node = cur_node->next;	
		}
		
		cur_node->next = NULL;
	}	
}

void list_de_by_msgid(list_node **p_head, int msg_id)
{
	list_node *tmp_node = *p_head;
	list_node *cur_node = NULL;
	
	if(tmp_node == NULL)
	{
		USART_OUT(USART1, "list_de_by_msgid_return\r\n");
		return;
	}
	else
	{
		if(tmp_node->msg.msg_id == msg_id)
		{
			USART_OUT(USART1, "list_de_by_elem333=%d\r\n", tmp_node->msg.msg_id);
			*p_head = (*p_head)->next;
			free(tmp_node);
			USART_OUT(USART1, "list_de_by_elem111=%d\r\n", tmp_node->msg.msg_id);
		}
		else
		{
			while(tmp_node != NULL)
			{
				cur_node = tmp_node;
				cur_node = tmp_node->next;
				if(cur_node->msg.msg_id == msg_id)
				{
					tmp_node->next = cur_node->next;
//					free(tmp_node);
					free(cur_node);
					USART_OUT(USART1, "list_de_by_elem222=%d\r\n", tmp_node->msg.msg_id);
				}
				tmp_node = tmp_node->next;		
			}		
		}
	}
}


void list_de_by_elem1(list_node *p_head, int msg_id)
{
	list_node *tmp_node = p_head;
	list_node *cur_node = NULL;
	
	if(p_head == NULL)
	{
		return;
	}
	else
	{
		if(tmp_node->msg.msg_id == msg_id)
		{
			p_head = tmp_node->next;
			free(tmp_node);
		}
		else
		{
			while(tmp_node != NULL)
			{
				cur_node = tmp_node;
				cur_node = tmp_node->next;
				if(cur_node->msg.msg_id == msg_id)
				{
					tmp_node->next = cur_node->next;
				}
				tmp_node = tmp_node->next;
			}
		}
	}
}



mqtt_msg_s *list_get_addr_by_msgid(list_node *p_head, int msg_id)
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
		USART_OUT(USART1, "msg_id=%d=list_get_addr_by_msgid=%d\r\n", msg_id, &(p_head->msg));
	}
	
	return &(p_head->msg);
}


mqtt_msg_s *list_get_addr_by_status(list_node *p_head, int status)
{
	if(NULL == p_head)
	{
		return NULL;
	}
	
	while((p_head->msg.status  != status) && (NULL != p_head->next))//判断是否到链表的尾部，以及存在要找的元素
	{
		p_head = p_head->next;
	}
	
	if((p_head->msg.status != status) && (p_head != NULL))//未找到
	{
		return NULL;
	}
	
	if(p_head->msg.status == status)
	{
		USART_OUT(USART1, "status=%d===list_get_addr_by_status=%d\r\n", status, &(p_head->msg));
	}
	
	return &(p_head->msg);
}


int list_modify_elem(list_node **p_node, int msg_id, int status)
{
	list_node *p_head;
	p_head = *p_node;
	
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
		USART_OUT(USART1, "p_head->msg.msg_id=%d\r\n", p_head->msg.msg_id);
		USART_OUT(USART1, "p_head->msg.status=%d\r\n", p_head->msg.status);
		USART_OUT(USART1, "list_get_elem_addr=%d=%d\r\n", msg_id, &(p_head->msg));
	}
	
	return 1;
}

mqtt_msg_s *list_find_min_val(list_node **p_head)
{
	list_node *min;
	list_node *temp = (*p_head);
	
	min = (*p_head)->next;
	
	while(NULL != temp)
	{
		if(temp->msg.msg_id < min->msg.msg_id)
		{
			min = temp;
		}
		temp = temp->next;
	}
	
	USART_OUT(USART1, "list_min_val=%d\r\n", min->msg.msg_id);
	
	return &(min->msg);
}


void l_test(char **p)
{
	char *b;
	char *a = *p;
	
	
	b = (char*)malloc(sizeof(char));
	
	
}

void list_test(list_node **list)
{
	int list_status = 0;
	int size = 0;
	
//	list_node *list = NULL;
	mqtt_msg_s mqtt_msg;
	mqtt_msg_s mqtt_msg1;
	mqtt_msg_s mqtt_msg2;
	mqtt_msg_s mqtt_msg3;
	mqtt_msg_s mqtt_msg4;
	mqtt_msg_s mqtt_msg5;
	mqtt_msg_s *mqtt_msg6;
	mqtt_msg_s *mqtt_msg7;
	mqtt_msg_s *mqtt_msg8;
	mqtt_msg_s *mqtt_msg9;
	
	memset(&mqtt_msg, 0, sizeof(mqtt_msg_s));
	memset(&mqtt_msg1, 0, sizeof(mqtt_msg_s));
	memset(&mqtt_msg2, 0, sizeof(mqtt_msg_s));
	memset(&mqtt_msg3, 0, sizeof(mqtt_msg_s));
	memset(&mqtt_msg4, 0, sizeof(mqtt_msg_s));
	
	char a[55]="aaa";
	char b[55]="bbb";
	char c[55]="ccc";
	char d[55]="ddd";
	char e[55]="eee";
	char f[55]="fff";
	char g[55]="ggg";
	
	
//	mqtt_msg1.payload = a;
	memcpy(mqtt_msg1.payload, a, 3);
	mqtt_msg1.payloadlen = 3;
	mqtt_msg1.msg_id = 1;
	mqtt_msg1.status = 22;
	
//	mqtt_msg2.payload = b;
	memcpy(mqtt_msg2.payload, b, 3);
	mqtt_msg2.payloadlen = 3;
	mqtt_msg2.msg_id = 222;
	mqtt_msg2.status = 33;
	
//	mqtt_msg3.payload = c;
	memcpy(mqtt_msg3.payload, c, 3);
	mqtt_msg3.payloadlen = 3;
	mqtt_msg3.msg_id = 3;
	mqtt_msg3.status = 44;
	
//	mqtt_msg4.payload = d;
	memcpy(mqtt_msg4.payload, d, 3);
	mqtt_msg4.payloadlen = 3;
	mqtt_msg4.msg_id = 4;
	mqtt_msg4.status = 55;
	

	list_insert_last(list, mqtt_msg1);
	list_travese(list);
	
	mqtt_msg7 = list_get_addr_by_status(*list, 55);
	USART_OUT(USART1, "mqtt_msg7->status=%d\r\n", mqtt_msg7->status);
	USART_OUT(USART1, "mqtt_msg7->payload=%s\r\n", mqtt_msg7->payload);
	
//	list_travese(mqtt_send_list);
	
	size = list_size(*list);
//	list_status = list_is_empty(mqtt_send_list);
//	
	list_insert_last(list, mqtt_msg2);
	list_travese(list);
	size = list_size(*list);
	
	list_insert_last(list, mqtt_msg3);
	list_travese(list);
	size = list_size(*list);
	
	
	list_insert_last(list, mqtt_msg4);
	list_travese(list);
	size = list_size(*list);
	
	
	
	mqtt_msg6 = list_get_addr_by_status(*list, 55);
	USART_OUT(USART1, "mqtt_msg6->status=%d\r\n", mqtt_msg6->status);
	USART_OUT(USART1, "mqtt_msg6->payload=%s\r\n", mqtt_msg6->payload);
	
	
	mqtt_msg8 = list_find_min_val(list);
	USART_OUT(USART1, "mqtt_msg8->msg_id=%d\r\n", mqtt_msg8->msg_id);
	USART_OUT(USART1, "mqtt_msg8->payload=%s\r\n", mqtt_msg8->payload);
	
	mqtt_msg9 = list_get_addr_by_msgid(*list, 222);
	USART_OUT(USART1, "mqtt_msg9->msg_id=%d\r\n", mqtt_msg9->msg_id);
	USART_OUT(USART1, "mqtt_msg9->payload=%s\r\n", mqtt_msg9->payload);
	
	size = list_size(*list);
	
//	list_insert_last(&list, mqtt_msg4);
//	list_travese(list);
//	
//	list_insert_last(&list, mqtt_msg4);
//	list_travese(list);
//	
//	list_insert_last(&list, mqtt_msg4);
//	list_travese(list);
	
	size = list_size(*list);
	
//	list_travese(mqtt_send_list);

//	list_de_by_elem(&mqtt_send_list, 6);
//	list_travese(mqtt_send_list);

//	list_de_by_elem(&mqtt_send_list, 3);
//	list_travese(mqtt_send_list);
//	
//	list_de_by_elem(&mqtt_send_list, 2);
//	list_travese(mqtt_send_list);

//	list_de_by_elem(&mqtt_send_list, 1);
//	list_travese(mqtt_send_list);


	list_get_addr_by_status(*list, 3);
	USART_OUT(USART1, "mqtt_send_list->msg.payload=%s\r\n", mqtt_send_list->msg.payload);	
	USART_OUT(USART1, "mqtt_send_list->msg.msg_id=%d\r\n", mqtt_send_list->msg.msg_id);
	
//	list_modify_elem(mqtt_send_list, 1, 8);
//	list_travese(&mqtt_send_list);
//		
//	list_de_last(&mqtt_send_list);
//	list_de_last(&mqtt_send_list);
//	list_de_last(&mqtt_send_list);
//	list_travese(mqtt_send_list);
//	size = list_size(mqtt_send_list);
//	list_status = list_is_empty(mqtt_send_list);
//	
//	list_insert_last(&mqtt_send_list, mqtt_msg4);
//	list_travese(&mqtt_send_list);
	
//	size = list_size(mqtt_send_list);
//	USART_OUT(USART1, "list_size=%d\r\n", size);
	

	
}

void list_test2(list_node **list)
{
	int size = 0;
	mqtt_msg_s mqtt_msg5;
	mqtt_msg_s *mqtt_msg8;
	mqtt_msg_s *mqtt_msg9;
	char e[55]="eee";
	
	
	
	memcpy(mqtt_msg5.payload, e, 3);
	mqtt_msg5.payloadlen = 3;
	mqtt_msg5.msg_id = 4;
	mqtt_msg5.status = PUBLISH;
	
	size = list_size(*list);
	list_insert_last(list, mqtt_msg5);
	list_travese(list);
	size = list_size(*list);
	USART_OUT(USART1, "list_size=%d\r\n", size);
	
	mqtt_msg8 = list_get_addr_by_status(*list, 55);
	USART_OUT(USART1, "mqtt_msg8->status=%d\r\n", mqtt_msg8->status);
	USART_OUT(USART1, "mqtt_msg8->payload=%s\r\n", mqtt_msg8->payload);

	
	mqtt_msg9 = list_get_addr_by_msgid(*list, 222);
	USART_OUT(USART1, "mqtt_msg9->msg_id=%d\r\n", mqtt_msg9->msg_id);
	USART_OUT(USART1, "mqtt_msg9->payload=%s\r\n", mqtt_msg9->payload);
}




