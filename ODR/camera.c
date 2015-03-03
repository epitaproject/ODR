#include <stdio.h>
#include <stdlib.h>
#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include <limits.h>

SDL_Surface* initsdl()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(836, 605, 32, SDL_HWSURFACE);
  return screen;
}

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

// permet de récupérer la couleur d'un pixel à la position x,y
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
// permet de modifier la couleur d'un pixel à la position x, y
// surface représente l'image sur laquelle on veut faire la modification et pixel représente la couleur du pixel
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
// un code degeu qui permet d'afficher un carre à l'endroit ou un objet noir est détecté
  void displaysdl(char*path, SDL_Surface *screen,int min,int max)
{
   SDL_Surface *picture;
    picture= IMG_Load(path);
    SDL_Surface *dst =SDL_SetVideoMode(picture->w,picture->h, 32, SDL_HWSURFACE);

    SDL_Rect rectangle;
    rectangle.x=0;
    rectangle.y=0;

    Uint32* myColor; //= (Uint32 *)((picture->pixels)[400000]);
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
            if( min< g+b && g+b < max && (j < picture->h -40))
            {
                color_dst = SDL_MapRGB(picture->format,180,230,100);
                somme_x = somme_x + i;
                cpt_x = cpt_x +1;
                somme_y = somme_y + j;
                cpt_y = cpt_y +1;
            }
            else
            {
            color_dst = SDL_MapRGB(picture->format,180,230,100);
            }
            putpixel(dst,i,j,color_dst);//((Uint32*)(picture->pixels))[j*(picture->h)+i]  );
        }
    }
      pos_x = somme_x/cpt_x;
    pos_y = somme_y/cpt_y;
     for(int i = pos_x - 70; i < pos_x + 70;i++)
    {
        for(int j = pos_y -70; j < pos_y +70;j++)
        {
                if(i<pos_x-60 | i > pos_x+60 | j<pos_y-60 | j > pos_y+60)
                {
                color_dst =  SDL_MapRGB(picture->format,255,0,0);
                putpixel(dst,i,j,color_dst);
                }
            //((Uint32*)(picture->pixels))[j*(picture->h)+i]  );
        }
    }

     printf("%s\n","-------");
     // affiche la position de l'objet
    printf("X : %d\n",somme_x/cpt_x);
    printf("Y : %d\n",somme_y/cpt_y);

    SDL_BlitSurface(dst, NULL,screen,&rectangle);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

//télécharge l'image
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
while(1)
{
   FILE*config=fopen("config","r");
   char* path=malloc(SHRT_MAX);
   int min; 
   int max;
   fscanf(config,"%s %i %i",path,min,max);
   
  // télécharge l'image qui se trouve sur la camera IP(l'adresse ip de la caméra est 172.21.1.200)
    //une fois qu'on récupère l'image on peut la traiter
    download(path,"toto.jpg");
    //traite l'image télécharger et affiche une nouvelle image qui correspont a celle du videoprojecteur
    displaysdl("toto.jpg",screen,min,max);
    i++;

}

    return 0;
}
