OPTIONS=-std=c99 -lcurl -lSDL -lSDL_mixer -lSDL_image `pkg-config --cflags gtk+-2.0`
POSTFIX=`pkg-config --libs gtk+-2.0`
all::
	gcc  $(OPTIONS) camera.c -o camera $(POSTFIX)
	gcc  $(OPTIONS) interface.c -o interface $(POSTFIX)
