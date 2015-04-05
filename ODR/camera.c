#include <stdio.h>
#include <stdlib.h>
#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include <SDL/SDL_rotozoom.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <limits.h>
#include <math.h>

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
	  color_dst =  SDL_MapRGB(dst->format,255,0,0);
	   putpixel(dst,x,y,color_dst);
	 for(int i = -1*r; i < r;i++)
    	{
        for(int j = -1*r; j < r;j++)
        {
		if(i+x >= 2 && i+x < dst->w-2 && j+y >= 2 && j+y< dst->h-2)
		{
               if((i*i) +(j*j) <= (r*r) && ( (i*i) +(j*j) >= ((r-6)*(r-6))   ))
		{

                putpixel(dst,i+x,j+y,color_dst);
		}
               }

        }
    }
}

void fill_me_away(SDL_Surface*surface,unsigned i,unsigned j,Uint32 mark)
{
  if((i+1<surface->w&&j+1<surface->h)&&(getpixel(surface,i,j)!=getpixel(surface,i,j+1)))
  {
    putpixel(surface,i,j,mark);
  }
  if((i+1<surface->w&&j+1<surface->h)&&(getpixel(surface,i,j)!=getpixel(surface,i+1,j+1)))
  {
    putpixel(surface,i,j,mark);
  }
  if((i+1<surface->w&&j+1<surface->h)&&(getpixel(surface,i,j)!=getpixel(surface,i+1,j)))
  {
    putpixel(surface,i,j,mark);
  }
}



int is_out_circle(int x, int y, int i ,int j,int r)
{
    return ((i-x)*(i-x)) + ((j-y)*(j-y)) >= ((r)*(r));
}

//indique si un point est sur le bouchon d'une des bouteilles
int is_out_circle_tab(int* x,int* y, int i, int j, int r)
{
    //printf("%s\n","________________");
    int in_one_circle = 1;
    int cpt = 0;
    while(cpt <=1 && in_one_circle == 1)
    {
      //  printf("%d\n",cpt);
        in_one_circle = is_out_circle(x[cpt],y[cpt],i,j,r);

        cpt++;
    }
    return in_one_circle;

}

void exclude_bottles(SDL_Surface *dst, int r,int* x_tab, int* y_tab)
{
	 Uint32 color_dst;
	 for(int i = 0; i < dst->w;i++)
    	{
        for(int j = 0; j < dst->h;j++)
        {
               if(  is_out_circle_tab(x_tab,y_tab,i,j,r))
                {
                    color_dst =  SDL_MapRGB(dst->format,255,255,255);
                    putpixel(dst,i,j,color_dst);
                }

        }
    }
}

int** init_int_Tab(int x)
{
    int** marqTab = malloc(x*sizeof(int*));
    for(int i = 0; i < x;i++)
    {
        marqTab[i] = malloc(x*sizeof(int));
    }
    for(int j = 0; j < x;j++)
    {
        for(int i = 0; i < x;i++)
        {
            marqTab[i][j] = 0;
        }
    }
    return marqTab;
}

void reset_int_Tab(int** tab,int x)
{
       for(int j = 0; j < x;j++)
        {
            for(int i = 0; i < x;i++)
            {
                tab[i][j] = 0;
            }
        }
}

void seuil_tab(int** tab, int seuil, int x)
{
     for(int j = 0; j < x;j++)
        {
            for(int i = 0; i < x;i++)
            {
                  if(tab[i][j] < seuil)
            {
                tab[i][j] = 1;
            }
            else
            {
                tab[i][j] = 0;
            }
            }
        }
}

void print_tab(int** tab)
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            printf("%d |",tab[j][i]);
        }
         printf("%s\n","");
    }
     printf("%s\n","");
}

void extract_flash_info(SDL_Surface * src,int** marqTab)
{
    Uint32* myColor;
    Uint8* r,g,b;

    for(int i = 5; i < 36; i++)
    {
        for(int j = 5; j < 36; j++)
        {
            myColor = getpixel(src,i,j);
            SDL_GetRGB(myColor,src->format, &r ,&g,&b);
            marqTab[(i-5)/8][(j-5)/8] += g;

        }
    }

}

int flash_is_valid(int** marqTab)
{
    return (marqTab[0][0]&&marqTab[3][0]&&marqTab[0][3]&&marqTab[3][3]==0);
}

SDL_Surface* rotate_surface(SDL_Surface *picture,double angle)
{

    SDL_Surface* dst = SDL_SetVideoMode(43,43, 32, SDL_HWSURFACE);
    SDL_Surface* tmp = SDL_SetVideoMode(43,43, 32, SDL_HWSURFACE);
    SDL_Rect rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    SDL_FillRect(dst, NULL, 0xffffff);
    dst = rotozoomSurface(picture,angle,1,0);
    rectangle.x = picture->w/2 -dst->w/2;
    rectangle.y = picture->w/2 - dst->h/2;
    SDL_BlitSurface(dst,NULL,tmp,&rectangle);
    SDL_FreeSurface(dst);
    return tmp;
}

void orientate_surface(SDL_Surface *src)
{

    int valid = 0;
    int i = 1;
    int **marqTab = init_int_Tab(4);
    extract_flash_info(src,marqTab);
    seuil_tab(marqTab,10500,4);
    print_tab(marqTab);
    valid = flash_is_valid(marqTab);
    while(valid == 0)
    {
        reset_int_Tab(marqTab,4);
        SDL_Surface* p;
        p = rotate_surface(src,i*90);
        extract_flash_info(p,marqTab);
      seuil_tab(marqTab,10500,4);
        valid = flash_is_valid(marqTab);
        printf("%d\n",valid);
        i++;
         print_tab(marqTab);
        SDL_FreeSurface(p);
        sleep(5);
    }
    printf("%s\n","find");


}
//renvoi une surface seuillé. Attention penser à libérer la surface src
SDL_Surface* seuillage(SDL_Surface *src, int min, int max)
{
    SDL_Surface *dst =SDL_SetVideoMode(src->w,src->h, 32, SDL_HWSURFACE);

    Uint32* color_src,color_dst;
    Uint8* r,g,b;
      for(int i = 0; i < src->w;i++)
    {
        for(int j = 0; j < src->h ;j++)
        {
            color_src = getpixel(src,i,j);
            SDL_GetRGB(color_src,src->format, &r ,&g,&b);
            if( min<= b && b < max)
            {
                color_dst = SDL_MapRGB(src->format,0,0,0);
            }
            else
            {
                color_dst = SDL_MapRGB(src->format,255,255,255);
	          }
            putpixel(dst,i,j,color_dst);
        }
    }
    return dst;
}

int* get_pos(SDL_Surface* src, int color)
{
    int* tab = malloc(sizeof(int)*2);
    int somme_x = 0;
    int somme_y = 0;
    int nb = 0;
    Uint32* color_src,color_dst;
    Uint8* r,g,b;
    for(int i = 10; i < src->w-10;i++)
    {
        for(int j = 10; j < src->h-10 ;j++)
        {
            color_src = getpixel(src,i,j);
            SDL_GetRGB(color_src,src->format, &r ,&g,&b);
            if(b == color)
            {
               somme_x += i;
               somme_y += j;
               nb++;
            }
        }
    }
    if(nb > 2000)
    {
    tab[0] = somme_x/nb;
    tab[1] = somme_y/nb;
    }
    else
    {
        tab[0] = -1;
        tab[1] = -1;
    }
    return tab;
}

int** appel_get_pos(SDL_Surface* src, int nb_flash)
{
    int **posTab = malloc(nb_flash*sizeof(int*));
    for(int i = 0; i < nb_flash;i++)
    {
        posTab[i] = get_pos(src,i);
    }
    return posTab;
}

int getcfc(SDL_Surface *src,int x_p, int y_p, int** marqTab, int ncfc)
{
   // printf("%s\n","");
    //printf("x: %i\n",x_p);
    //printf("y: %i\n",y_p);
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
	//printf("%s\n","f");
	//tant que la file n'est pas vide
	while(q.size > 0)
	{
	//printf("%s\n","2");
		tmp = q.pop(&q);
		x = tmp.x;
		y = tmp.y;
		putpixel(src,x,y,color_dst);

		for(int i = -1;i <= 1;i++)
		{
			for(int j = -1;j <= 1;j++)
			{
			//printf("%d\n",x);
			//printf("%d\n",y);
				myColor = getpixel(src,x+i,y+j);
				SDL_GetRGB(myColor,src->format,&r,&g,&b);
				//si l'élément n'est pas marqué et s'il est noir on l'enfile
				if(b <1  && marqTab[x+i][y+j] == 0 && x+i <src->w-10 && x+i > 0 && y+j > 0 && y+j< src->h)
				{
					tmp.x = x+i;
					tmp.y = y+j;
					q.push(&q,tmp);
					marqTab[x+i][y+j] = 1;
					 //  printf("%i\n",x_p);
                   // printf("%i\n",y);
				}
		//		printf("%s\n","4");
			}
		}
	}

}

int appel(SDL_Surface *src)
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

     for(int j = 10; j < src->h-10;j++)
    {
        //printf("%s\n","bla");
        for(int i = 10; i < src->w-10;i++)
        {
            //printf("%s\n","bla");
            myColor = getpixel(src,i,j);

            SDL_GetRGB(myColor,src->format,&r,&g,&b);


            if(marqTab[i][j] ==0 && (b)<1 )
            {
              //  printf("%s\n","prout");
                getcfc(src,i,j,marqTab,ncfc);
                ncfc+= 1;
                //printf("fdsfsfdsfdsfdsfdsffs :  %i\n",ncfc);
            }
        }
    }
      for(int i = 0; i < src->w;i++)
    {
        free(marqTab[i]);
    }
    return ncfc;
}

void print_line(SDL_Surface* src,int x_1, int y_1, int x_2, int y_2)
{
    Uint32 *color_dst;
      int eq_x,eq_y;
	  double coeff,affine;
	  coeff = (double)((y_1-y_2)/(x_1-x_2));
	  affine = y_2 - coeff*x_2;
    // Si la source est AVANT la destination
    if(x_2<x_1)
    {
	  for(eq_x = x_2; eq_x <= x_1;eq_x++)
	  {

              eq_y= (int)(coeff*eq_x+affine);
            color_dst = SDL_MapRGB(src->format,0,255,255);
		      putpixel(src,eq_x,eq_y,color_dst);
		      putpixel(src,eq_x+1,eq_y,color_dst);
		      putpixel(src,eq_x-1,eq_y,color_dst);
		      putpixel(src,eq_x,eq_y+1,color_dst);
		      putpixel(src,eq_x,eq_y-1,color_dst);

	  }
    }
    //SI la destination est AVANT la source
    else
        {
             for(eq_x = x_1; eq_x <= x_2;eq_x++)
	  {
                printf("x: %i",eq_x);
                printf("y: %i",eq_y);
              eq_y= (int)(coeff*eq_x+affine);
            color_dst = SDL_MapRGB(src->format,0,255,255);
		      putpixel(src,eq_x,eq_y,color_dst);
		      putpixel(src,eq_x+1,eq_y,color_dst);
		      putpixel(src,eq_x-1,eq_y,color_dst);
		      putpixel(src,eq_x,eq_y+1,color_dst);
		      putpixel(src,eq_x,eq_y-1,color_dst);

	  }
        }


}

double calculAngle(double pixelX, double pixelY, double centreX,
		   double centreY, double cibleX, double cibleY)
{
  // Normes de vecteur (sommes de 2 vecteurs)
  double distanceB = sqrt((cibleX-pixelX)*(cibleX-pixelX)+
			  (cibleY-pixelY)*(cibleY-pixelY));
  double distanceA = sqrt((cibleX-centreX)*(cibleX-centreX)+
			  (cibleY-centreY)*(cibleY-centreY));
  double distanceC = sqrt((centreX-pixelX)*(centreX-pixelX)+
			  (centreY-pixelY)*(centreY-pixelY));
  // Al Kashi angle B
  double angleRadian= acos(-(distanceB*distanceB - distanceA*distanceA -
		 distanceC*distanceC)/(2*distanceA*distanceC));
	return (180*angleRadian/3.14);
}


void displaysdl(char*path, SDL_Surface *screen,int min,int max)
{
   SDL_Surface *picture;
    picture= IMG_Load(path);

            //SDL_Surface *t = SDL_SetVideoMode(src->w,src->h, 32, SDL_HWSURFACE);
            SDL_Surface *dst = seuillage(picture,0,90);//seuil l'image
          int nb_flashcode = appel(dst); //récupère le nombre de bouteille et colorie chacune des bouteille differement
        int** tab = appel_get_pos(dst,nb_flashcode);//récupère les position de chacune des bouteilles
        printf("nb_flashcode: %i\n",nb_flashcode);
        SDL_FillRect(dst, NULL, 0xffffff);
          for(int i = 0;i < nb_flashcode;i++)
          {
                if(tab[i][0]!= -1)
                {
                   printf("%s\n","----------------------");
              //  printf("c_x: %i\n",tab[i][0]);
                //printf("c_y: %i\n",tab[i][1]);
                print_cercle(dst,100,tab[i][0], tab[i][1]);
                }
          }
          //printf("%s\n","bla");

       //  print_line(dst,200,90,700,90);
        // printf("%s\n","blo");
          /*SDL_Surface **flashTab = malloc(nb_flashcode*sizeof(SDL_Surface*));
          for(int i = 0; i < nb_flashcode;i++)
          {
            flashTab[i] = SDL_CreateRGBSurface(0,43,43,32,0,0,0,0);
          }*/

           // free(tab);

                SDL_BlitSurface(dst,NULL,screen,NULL);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    //  sleep(100);


SDL_FreeSurface(picture);
    SDL_FreeSurface(dst);
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}


void download(char *src,char* des)
{
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
    int config=open("config",O_RDONLY);
    read(config,(void*)string,SHRT_MAX);
    sscanf(string,"%s %i %i %i",path,&max,&min,&exit);
    //sleep(100);
    while(1)
    {
        if(exit){return 0;}
       download("http://172.21.1.200/cgi-bin/jpg/image.cgi?resolution=704x576&dummy=1422852582922","toto.jpg");
        displaysdl("toto.jpg",screen,min,max);
       // sleep(1);
    }

    return 0;
}
