#include "library.h"
queue *queue_init()
{
  queue* ret=malloc(sizeof(queue));
  ret->head=NULL;
  ret->tail=NULL;
}

void ins(queue* q,void*data)
{ 
  cell* nc=malloc(sizeof(cell));
  nc->data=data;
  
  if(q->head==NULL)
  {
    nc->prev=NULL;
    nc->next=NULL;
    q->tail=nc;
    q->head=q->tail;
  }
  else
  {
  nc->prev=q->tail;
  q->tail->next=nc;
  q->tail=q->tail->next;
  }
}

void * rem(queue *q)
{
  if(q->head==NULL)
  {return NULL;}
void*rete=q->head->data;
if(q->head==q->tail)
{
  free(q->head);
  q->head=NULL;
  q->tail=NULL;
}
else
{
  q->head=q->head->next;
  free(q->head->prev);
  q->head->prev=NULL;
}
return rete;
}

void free_queue(queue*q)
{
  while(q->head)
  {
    rem(q);
  }
  free(q);
}
int main()
{
  queue *q=queue_init();
  int* i=(malloc(3*sizeof(int)));
  for(int it=0;it<3;it++)
  {
    i[it]=it;
    ins(q,(void*)&it);
  }
  free_queue(q);
}