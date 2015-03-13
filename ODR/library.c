
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
  void * rete=q->head->data;
 q->head=q->head->next;
 free(q->head->prev);
 return rete;
}