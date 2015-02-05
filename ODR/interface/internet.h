#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>

void download(char *src,char* des);
void displaysdl(char*path, SDL_Surface *screen);
SDL_Surface *initsdl();
