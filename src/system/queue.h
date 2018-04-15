

#ifndef __QUEUE_H_
#define __QUEUE_H_
#include <stdbool.h>
#include "bsp.h"



#define QUEUE_MAXSIZE		5


typedef struct
{
	char *data;
	char **buff;
	int front;
	int rear;
	int size;
}sp_queue, *p_que;









bool queue_init(sp_queue *que, int maxsize);


void queue_test(void);







#endif
