

#include <string.h>		
#include <stdio.h>
#include "list.h"







void list_init(list_node **p_head)
{
	*p_head = NULL;
	
}

list_node *list_node_init(char *data)
{
	
	list_node *p_node;
	
	p_node = (list_node *)malloc(sizeof(list_node));
	if(p_node == NULL)
	{
		return NULL;
	}
	
	p_node->data = data;
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


int list_is_empty(list_node *p_head)
{
	if(p_head == NULL)
	{
		return 1;
	}
		
	return 0;
}


int list_insert_last(list_node **p_head, char *data)
{
	
	list_node *p_insert;
//	list_node *p_head = p_node;
	list_node *p_tmp = *p_head;	//定义一个临时链表用来存放第一个节点
	
	p_insert = (list_node *)malloc(sizeof(list_node));
	memset(p_insert, 0, sizeof(list_node));
	p_insert->data = data;
	
	if(NULL == p_head)
	{
		*p_head = list_node_init(data);
		
	}
	else
	{
		while(p_tmp->next != NULL)
		{
			p_tmp = p_tmp->next;
		}
		p_tmp->next = list_node_init(data);;
		return 1;
	}
}















