#ifndef FIFO_H
#define FIFO_H
#include<stdint.h>
#include"common.h"
#include "gd32f1x0.h"

#define FIFO_MAX 256
#define QElemType uint8_t
#define Queue_State uint8_t
#define Queue_Is 1
#define Queue_No 0

typedef struct Fifo_Queue{
	uint32_t front;
	uint32_t rear;
	uint16_t count;
	uint8_t  buf[FIFO_MAX];
}Queue;

uint32_t InitQueue(Queue *Q);

Queue_State Is_Queue_Full(Queue *Q);

Queue_State Is_Queue_Empty(Queue *Q);

uint32_t EnQueue(Queue *Q,QElemType x);

uint32_t DeQueue(Queue *Q,QElemType *x);

uint32_t Get_Queue_len(Queue *Q);

#endif

