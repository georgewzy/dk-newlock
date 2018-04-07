

#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "bsp.h"






typedef struct
{
	uint8_t queue_array[100];
	int front;
	int rear;
}sp_queue;



#endif
