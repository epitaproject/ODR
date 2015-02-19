#include <stdio.h>
#include <stdlib.h>
#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>


SDL_Surface* initsdl()
{

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Surface *screen = SDL_SetVideoMode(836, 605, 32, SDL_HWSURFACE);
  return screen;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return *p;
        else
            return *p;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}



void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
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
Uint32* get_pixel32( SDL_Surface *surface, int x, int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
   // printf("%d\n",sizeof(pixels));
    //Get the requested pixel
    return pixels[ (y *surface->w) +x];
}
void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels ;
    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}



  void displaysdl(char*path, SDL_Surface *screen)
{
   SDL_Surface *picture;
    picture= IMG_Load(path);
    SDL_Surface *dst =SDL_SetVideoMode(picture->w,picture->h, 32, SDL_HWSURFACE);

    SDL_Rect rectangle;
    rectangle.x=0;
    rectangle.y=0;

    Uint32* myColor; //= (Uint32 *)((picture->pixels)[400000]);
    Uint8* r,g,b;


    for(int i = 0; i < picture->w;i++)
    {
        for(int j = 0; j < picture->h;j++)
        {
         //   SDL_GetRGB(get_pixel32(picture,i,j),picture->format,&r,&g,&b);
        //printf("%d\n",i);
        //printf("%d\n",j);
            myColor = get_pixel32(picture,i,j);
         //  myColor = SDL_MapRGB(picture->format,200,200,200);
            put_pixel32(dst,i,j,myColor);//((Uint32*)(picture->pixels))[j*(picture->h)+i]  );
        }
    }

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
while(i<1000)
{
    printf("%d\n",i);
    //download("http://172.21.1.200/cgi-bin/jpg/image.cgi?resolution=704x576&dummy=1422852582922","toto.jpg");

    displaysdl("toto.jpg",screen);
    i++;

}

    return 0;
}
