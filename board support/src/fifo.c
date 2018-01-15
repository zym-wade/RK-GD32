#include"fifo.h"


/**
  \fn          uint32_t InitQueue(Queue *Q)
  \brief       Initialize Queue
	\param[in]  Q: specify the Queue to be configured
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
uint32_t InitQueue(Queue *Q)
{
	Q->front = Q->rear = 0;
	Q->count = 0;
	return SUCCESS;
}

/**
  \fn          Queue_State Is_Queue_Full(Queue *Q)
  \brief       Determines whether the queue is full
	\param[in]  Q: specify the Queue to be judge
  \returns
   - \b  Queue_Is:  Queue Full
   - \b  Queue_No1: Queue not Full
*/
Queue_State Is_Queue_Full(Queue *Q)
{
	if( (Q->rear+1)%FIFO_MAX == Q->front ){
		return Queue_Is;
	}
	return Queue_No;
}

/**
  \fn          Queue_State Is_Queue_Empty(Queue *Q)
  \brief       Determines whether the queue is empty
	\param[in]  Q: specify the Queue to be judge
  \returns
   - \b  Queue_Is:  Queue Empty
   - \b  Queue_No1: Queue not Empty
*/
Queue_State Is_Queue_Empty(Queue *Q)
{
	if( Q->rear == Q->front ){
		return Queue_Is;
	}
	return Queue_No;
}

/**
  \fn          uint32_t EnQueue(Queue *Q,QElemType x)
  \brief       EnQueue
	\param[in]  Q: specify the Queue to be configured
	\param[in]  x: the data to enqueue
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
uint32_t EnQueue(Queue *Q,QElemType x)
{
	if( Is_Queue_Full(Q)==Queue_Is ){
		return FAILED;
	}
	Q->buf[Q->rear] = x;
	Q->rear = (Q->rear+1)%FIFO_MAX;  /*designed not a queue,not necessary*/
	Q->count++;
	return SUCCESS;
}

/**
  \fn          uint32_t DeQueue(Queue *Q,QElemType *x)
  \brief       DeQueue
	\param[in]  Q: specify the Queue to be configured
	\param[in]  *x: the data to Dequeue and return 
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
uint32_t DeQueue(Queue *Q,QElemType *x)
{
	if( Is_Queue_Empty(Q)==Queue_Is ){
		return FAILED;
	}
	*x = Q->buf[Q->front];
	Q->front = (Q->front+1)%FIFO_MAX;
	Q->count--;
	return SUCCESS;
}

/**
  \fn          uint32_t Get_Queue_len(Queue *Q)
  \brief       get the length of Queue
	\param[in]   Q: specify the Queue to be get
  \return
   - \uint32_t Queue`length
*/
uint32_t Get_Queue_len(Queue *Q)
{
	return (Q->rear - Q->front + FIFO_MAX)%FIFO_MAX;
}
