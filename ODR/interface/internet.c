#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include<curl/curl.h>
#include <curl/easy.h>
#include <string.h>


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

void displaysdl(char*path)
{
    SDL_Rect rectangle;
    rectangle.x=0;
    rectangle.y=0;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *picture;
  SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);
  picture= IMG_Load(path);
  
  SDL_BlitSurface(picture, NULL,screen,&rectangle);

    SDL_Flip(screen);
}
/*
int main(int argc, char* argv[])
{
  download("http://192.168.1.194:194/cgi-bin/jpg/image.cgi?resolution=704x576&dummy=1422852582922","toto.jpg");
  return 0;
}*/
