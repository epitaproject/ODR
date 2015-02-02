CC=gcc
PREF= `pkg-config --cflags --libs gtk+-3.0`
SUFF=-lSDL -lSDL_mixer -lSDL_image -lcurl
DGB=
FILES=internet.c interface.c 
NAME=interface
ALL:
	$(CC) $(PREF) $(DBG) $(FILES) -o $(NAME) $(SUFF)
