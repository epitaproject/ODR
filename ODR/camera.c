#include <stdio.h>
#include <stdlib.h>
#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <limits.h>
#include"shapes.h"

struct pos
{
    int x,y;
};

typedef struct Node {
    struct pos item;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* head;
    Node* tail;

    void (*push) (struct Queue*, struct pos); // add item to tail
    // get item from head and remove it from queue
    struct pos (*pop) (struct Queue*);
    // get item from head but keep it in queue
    int (*peek) (struct Queue*);
    // display all element in queue
    void (*display) (struct Queue*);
    // size of this queue
    int size;
} Queue;

void push (Queue* queue, struct pos item) {
    // Create a new node
    Node* n = (Node*) malloc (sizeof(Node));
    n->item = item;
    n->next = NULL;

    if (queue->head == NULL) { // no head
        queue->head = n;
    } else{
        queue->tail->next = n;
    }
    queue->tail = n;
    queue->size++;
}

struct pos pop (Queue* queue) {
    // get the first item
    Node* head = queue->head;
    struct pos item = head->item;
    // move head pointer to next node, decrease size
    queue->head = head->next;
    queue->size--;
    // free the memory of original head
    free(head);
    return item;
}

struct pos peek (Queue* queue) {
    Node* head = queue->head;
    return head->item;
}

void display (Queue* queue) {
    printf("\nDisplay: ");
    // no item
    if (queue->size == 0)
        printf("No item in queue.\n");
    else { // has item(s)
        Node* head = queue->head;
        int i, size = queue->size;
        printf("%d item(s):\n", queue->size);
        for (i = 0; i < size; i++) {
            if (i > 0)
                printf(", ");
            printf("%d", head->item.y);
            head = head->next;
        }
    }
    printf("\n\n");
}

Queue createQueue () {
    Queue queue;
    queue.size = 0;
    queue.head = NULL;
    queue.tail = NULL;
    queue.push = &push;
    queue.pop = &pop;
    queue.peek = &peek;
    queue.display = &display;
    return queue;
}


SDL_Surface* initsdl()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(704, 576, 32, SDL_HWSURFACE);
  return screen;
}


Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(Uint32 *)p;
  } 
    return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
  Uint8 *p = pixelref(surface, x, y);     
  switch(surface->format->BytesPerPixel) {
  case 1:
    *p = pixel;
    break;
  case 2:
    *(Uint16 *)p = pixel;
    break;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *(Uint32 *)p = pixel;
    break;
  } 
}

void print_cercle(SDL_Surface *dst, int r,int x, int y)
{
	 Uint32 color_dst;
	 for(int i = -1*r; i < r;i++)
    	{
        for(int j = -1*r; j < r;j++)
        {
		if(i+x >= 0 && i+x < 704 && j+y >= 0 && j+y<576)
		{
               if((i*i) +(j*j) <= (r*r) && ( (i*i) +(j*j) >= ((r-6)*(r-6))   ))
		{
                color_dst =  SDL_MapRGB(dst->format,255,0,0);
                putpixel(dst,i+x,j+y,color_dst);
		}
               }

        }
    }
}                                   
void appel(SDL_Surface *src)
{
    int ncfc = 0;
    int** marqTab = malloc((src->w)*sizeof(int*));
    for(int i = 0; i < src->w;i++)
    {
        marqTab[i] = malloc((src->h)*sizeof(int));
    }

    for(int j = 0; j < src->h;j++)
    {
        for(int i = 0; i < src->w;i++)
        {
            marqTab[i][j] = 0;
        }
    }
    Uint32 *myColor;
    Uint8* r,g,b;
     for(int j = 0; j < src->h;j++)
    {
        for(int i = 0; i < src->w;i++)
        {
            myColor = getpixel(src,i,j);
            SDL_GetRGB(myColor,src->format,&r,&g,&b);


            if(marqTab[i][j] ==0 && (g+b)<10 )
            {
                printf("%s\n","prout");
                getcfc(src,i,j,marqTab,ncfc);
                ncfc += 1;
            }
        }
    }
}

void getcfc(SDL_Surface *src,int x_p, int y_p, int** marqTab, int ncfc)
{
    Uint32 color_dst = SDL_MapRGB(src->format,ncfc,ncfc,ncfc);
	Uint32 *myColor;
	Uint8 * r,g,b;
	int x;
	int y;
	Queue q = createQueue();
	struct pos tmp;
	tmp.x = x_p;
	tmp.y = y_p;
	q.push(&q,tmp);
	marqTab[x_p][y_p] = 1;
	//tant que la file n'est pas vide
	while(q.size > 0)
	{
		tmp = q.pop(&q);
		x = tmp.x;
		y = tmp.y;
		putpixel(src,x,y,color_dst);
		for(int i = -1;i <= 1;i++)
		{
			for(int j = -1;j <= 1;j++)
			{
				myColor = getpixel(src,x+i,y+j);
				SDL_GetRGB(myColor,src->format,&r,&g,&b);
				//si l'élément n'est pas marqué et s'il est noir on l'enfile
				if(r+g+b < 10 && marqTab[x+i][y+j] == 0)
				{
					tmp.x = x+i;
					tmp.y = y+j;
					q.push(&q,tmp);
					marqTab[x+i][y+j] = 1;
				}
			}
		}
	}

}

  void displaysdl(char*path, SDL_Surface *screen,int min,int max)
{
   SDL_Surface *picture;
    picture= IMG_Load(path);
    SDL_Surface *dst =SDL_SetVideoMode(picture->w,picture->h, 32, SDL_HWSURFACE);

    SDL_Rect rectangle;
    rectangle.x=0;
    rectangle.y=0;

    Uint32* myColor;
    Uint8* r,g,b;
 Uint32 color_dst;
    int somme_x=0;
    int cpt_x = 1;
    int somme_y=0;
    int cpt_y = 1;
    int pos_x = 0;
    int pos_y = 0;

    for(int i = 0; i < picture->w;i++)
    {
        for(int j = 0; j < picture->h ;j++)
        {
            myColor = getpixel(picture,i,j);
            SDL_GetRGB(myColor,picture->format, &r ,&g,&b);
            if( min<= g+b && g+b < max && (j < picture->h -40))
            {
                color_dst = SDL_MapRGB(picture->format,0,0,0);
                somme_x = somme_x + i;
                cpt_x = cpt_x +1;
                somme_y = somme_y + j;
                cpt_y = cpt_y +1;
                putpixel(dst,i,j,color_dst);
            }
            else
            {
                color_dst = SDL_MapRGB(picture->format,180,230,100);
	          }
            putpixel(dst,i,j,color_dst);
        }
    }

    pos_x = somme_x/cpt_x;
    pos_y = somme_y/cpt_y;

    appel(dst);

     print_cercle(dst, 70,pos_x, pos_y);


//
point *p1=malloc(sizeof(point));
point *p2=malloc(sizeof(point));
p1->x=pos_x-70;
p1->y=pos_y-70;
p2->x=pos_x+100;
p2->y=pos_y+70;
line *l=malloc(sizeof(line));
l->p1=p1;
l->p2=p2;

         for(int z=pos_x-70;z<pos_x+70;z++)
         {
         	putpixel(dst,z,trace(l,z),1);
         	putpixel(dst,z,trace_orth(l,z),1);
         }
      
//      
     SDL_BlitSurface(dst, NULL,screen,&rectangle);
     SDL_UpdateRect(screen, 0, 0, 0, 0);
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}


void download(char *src,char* des) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char *url = src;
    char *outfilename = des;
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

int main()
{
SDL_Surface *screen;
  screen=initsdl();
  int i = 0;
  char* string=malloc(SHRT_MAX);
  char* path=malloc(SHRT_MAX);
  int exit=0;
  int min;
  int max;

while(1)
{
   int config=open("config",O_RDONLY);


   read(config,(void*)string,SHRT_MAX);


   sscanf(string,"%s %i %i %i",path,&max,&min,&exit);

   if(exit){return 0;}


    displaysdl("toto.jpg",screen,min,max);



}

    return 0;
}
