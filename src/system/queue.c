




#include <stdlib.h>
#include "usart.h"
#include "queue.h"




bool queue_init(sp_queue *que, int maxsize)
{
	int i= 0; 
	
	
	
//	que->buff = (char**)malloc(sizeof(char*)*maxsize);
// 	for(i=0; i<maxsize; i++)
//	{
//		que->buff[i] = (char*)malloc(sizeof(char)*100);
//		USART_OUT(USART1, "que->buff[%d]=%d\r\n", i, que->buff[i]);
//		if(que->buff[i] == NULL)
//		{
//			return false;
//		}
//	}
	
	
	que->data = (char*)malloc(sizeof(char)*maxsize);
	
	if(que->data == NULL)
	{
		return false;
	}
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
//		USART_OUT(USART1, "que->data=%d\r\n", que->data[i]);
		USART_OUT(USART1, "que->buff=%s\r\n", que->buff[i]);
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
//		*(que->buff + que->rear) = val;
		que->buff[que->rear] = val;
		
		USART_OUT(USART1, "addr=%d\r\n", *que->buff);
		USART_OUT(USART1, "aaaaaa=%d\r\n", que->rear);
		USART_OUT(USART1, "buff=%s\r\n", que->buff[que->rear]);
		que->rear = (que->rear + 1) % que->size;
		return true;
	}
	
	return false;
}

bool queue_en1(sp_queue *que, char val)
{
	if(!queue_full(que))
	{
		//两种写法
//		que->data[que->rear] = val;
		*(que->data + que->rear) = val;
		que->rear = (que->rear + 1) % que->size;
		return true;
	}
	
	return false;
}

bool queue_de1(sp_queue *que, char *val)
{
	if(!queue_empty(que))
	{
		//两种写法
//		*val = que->data[que->front];
		*val = *(que->data + que->front);

		que->front = (que->front + 1) % que->size;
		return true;
	}
	
	return false;
}

bool queue_de(sp_queue *que, char **val)
{
	if(!queue_empty(que))
	{
		*val = *(que->buff + que->front);
//		free(que->buff[que->front]);
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
	char d[55]="ddd";
	char e[55]="eee";
	char f[55]="fff";
	char g[55]="ggg";
	char z[12] = {0};
	char n[12] = {0};
	char *p;
	
	z[0]  = 0x31;
	z[1]  = 0x32;
	z[2]  = 0x33;
	z[3]  = 0x34;
	z[4]  = 0x35;
//	a[5]  = 0x36;
//	a[6]  = 0x37;
//	a[7]  = 0x38;
//	a[8]  = 0x39;
//	a[9]  = 0x40;
//	a[10] = 0x41;
//	a[11] = 0x42;
	
	
	queue_init(&qq, QUEUE_MAXSIZE);
	
	queue_traverse(&qq);
	queue_en1(&qq, z[0]);
	queue_en1(&qq, z[1]);
	queue_en1(&qq, z[2]);
	queue_en1(&qq, z[3]);
	queue_traverse(&qq);
	queue_de1(&qq, &n[0]);
	queue_traverse(&qq);
	queue_en1(&qq, z[4]);
	queue_traverse(&qq);
	queue_de1(&qq, &n[0]);
	queue_traverse(&qq);
	queue_en1(&qq, z[5]); 
	queue_traverse(&qq);
//	
	
	queue_en(&qq, a);
	queue_en(&qq, b);
	queue_traverse(&qq);
	queue_en(&qq, c);
	queue_en(&qq, d);
	queue_traverse(&qq);
	
	queue_de(&qq, &p);
	USART_OUT(USART1, "que->buff=%s\r\n", p);
	queue_traverse(&qq);
	queue_en(&qq, e);
	queue_traverse(&qq);
	queue_de(&qq, &p);
	USART_OUT(USART1, "que->buff=%s\r\n", p);
	queue_traverse(&qq);
	queue_en(&qq, f);
	queue_traverse(&qq);
//	queue_en(&qq, &a[3]);
//	queue_en(&qq, &a[4]);
//	queue_en(&qq, a[5]);
//	queue_en(&qq, a[6]);
//	queue_en(&qq, a[7]);
//	queue_en(&qq, a[8]);
//	queue_en(&qq, a[9]);
//	queue_en(&qq, a[10]);
	
	
//	queue_traverse(qq);
	
	
}








