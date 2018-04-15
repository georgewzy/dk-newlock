


#ifndef __LIST_H_
#define __LIST_H_
#include <stdbool.h>
#include "bsp.h"


typedef struct Node
{
	char *data;
	struct Node *next;
	
}list_node, *p_node;




#endif
