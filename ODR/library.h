
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

struct pos
{
  int x;
int y;
}

//enfile
void ins(queue* q,void*data);
static inline
void enfiler(queue*q,struct pos*pos)
{
ins(q,(void*)pos);
}

//défile
void * rem(queue *q);
static inline
struct pos*defiler(queue*q)
{
return (struct pos*) rem(q);
}

queue *queue_init();

void free_queue(queue *q);

#endif
