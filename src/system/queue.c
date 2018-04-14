




#include <stdlib.h>
#include "usart.h"
#include "queue.h"




bool queue_init(sp_queue *que, int maxsize)
{
	int i= 0;
	char **p;
	
	que->buff = (char**)malloc(sizeof(char*)*maxsize);
	if(que->buff == NULL)
	{
		return false;
	}
	
	
//	que->buff = (char**)malloc(sizeof(char*) * maxsize);
	
//	if(que->buff == NULL)
//	{
//		return false;
//	}
	que->front = que->rear = 0;
	que->size = maxsize;
	
	return true;
}


void queue_destroy(sp_queue *que)
{
	free(que->buff);
}

void queue_traverse(sp_queue *que)
{
	int i = que->front;
	
	while(i%que->size != que->rear)
	{
		
		i++;
	}
}

int queue_length(sp_queue *que)
{
	return (que->rear - que->front + QUEUE_MAXSIZE) % QUEUE_MAXSIZE;
}
	
bool queue_empty(sp_queue *que)
{
	return que->front == que->rear;
}

bool queue_full(sp_queue *que)
{
	return que->front == ((que->rear+1) % que->size);
}


bool queue_en(sp_queue *que, char *val)
{
	if(!queue_full(que))
	{
		strcpy(*(que->buff + que->rear), val);
//		*(que->buff + que->rear) = val;
		que->rear = (que->rear + 1) % que->size;
		return true;
	}
	
	return false;
}


bool queue_de(sp_queue *que, char **val)
{
	if(!queue_empty(que))
	{
		*val = *(que->buff + que->front);
		que->front = (que->front + 1) % que->size;
		return true;
	}
	
	return false;
}






void queue_test(void)
{
	sp_queue qq;
	char a[55]="aaa";
	char b[55]="bbb";
	char c[55]="ccc";
//	a[0]  = 0x31;
//	a[1]  = 0x32;
//	a[2]  = 0x33;
//	a[3]  = 0x34;
//	a[4]  = 0x35;
//	a[5]  = 0x36;
//	a[6]  = 0x37;
//	a[7]  = 0x38;
//	a[8]  = 0x39;
//	a[9]  = 0x40;
//	a[10] = 0x41;
//	a[11] = 0x42;
	
	
	queue_init(&qq, QUEUE_MAXSIZE);
	
	queue_en(&qq, a);
	queue_en(&qq, b);
	queue_en(&qq, c);
//	queue_en(&qq, &a[3]);
//	queue_en(&qq, &a[4]);
//	queue_en(&qq, a[5]);
//	queue_en(&qq, a[6]);
//	queue_en(&qq, a[7]);
//	queue_en(&qq, a[8]);
//	queue_en(&qq, a[9]);
//	queue_en(&qq, a[10]);
	
	
	queue_traverse(&qq);
	
	
}








