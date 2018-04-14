

#ifndef __QUEUE_H_
#define __QUEUE_H_
#include <stdbool.h>
#include "bsp.h"



#define QUEUE_MAXSIZE		10


typedef struct
{
	char **buff;
	int front;
	int rear;
	int size;
}sp_queue, *p_que;









void queue_test(void);







#endif
