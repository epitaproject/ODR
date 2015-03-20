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
#include"library.h"

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
                color_dst = SDL_MapRGB(picture->format,200,200,200);
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


 
  for(int i = 0; i < picture->w;i++)
    {
        for(int j = 0; j < picture->h ;j++)
	{ 
	  fill_me_away(dst,i,j,0);
        }
    }
    
     print_cercle(dst, 70,pos_x, pos_y);
    
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
   
  
    //download("http://172.21.1.200/cgi-bin/jpg/image.cgi?resolution=704x576&dummy=1422852582922","toto.jpg");
   
    displaysdl("toto.jpg",screen,min,max);

   

}

    return 0;
}
