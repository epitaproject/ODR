
#ifndef LIBRARY_H
#define LIBRARY_H
#include <stdlib.h>

typedef struct cell
{
  struct cell*prev;
  struct cell*next;
  void* data;
}cell;

typedef struct queue
{
  cell* head;
  cell* tail;
}queue;

//enfile
void ins(queue* q,void*data);

//défile
void * rem(queue *q);

queue *queue_init();

void free_queue(queue *q);

#endif