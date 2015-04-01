#include"shapes.h"

int trace(line *l,int t)
{
float an=l->p2->y-l->p1->y;
float ad=l->p2->x-l->p1->x;
float a=an/ad;
float b=(float)(l->p1->y)-a*(float)(l->p1->x);
return (int)(a*t+b);
}
int trace_orth(line *l,int t)
{
float an=l->p2->y-l->p1->y;
float ad=l->p2->x-l->p1->x;
float a=-(an/ad);
float b=(float)(l->p1->y)-a*(float)(l->p1->x);
return (int)(a*t+b);
}