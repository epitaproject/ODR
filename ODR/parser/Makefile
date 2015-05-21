 
OPTIONS=-std=c99 -lcurl -lSDL -lSDL_mixer -lSDL_image -lSDL_gfx -lm `pkg-config --cflags gtk+-2.0`
POSTFIX=`pkg-config --libs gtk+-2.0`

all::
	gcc -g code.c -o code $(POSTFIX) $(OPTIONS)