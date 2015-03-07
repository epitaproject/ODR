OPTIONS=-std=c99 -lcurl -lSDL -lSDL_mixer -lSDL_image `pkg-config --cflags gtk+-2.0`
POSTFIX=`pkg-config --libs gtk+-2.0`
all::
	gcc camera.c -o camera $(POSTFIX) $(OPTIONS)
	gcc interface.c -o interface $(POSTFIX) $(OPTIONS)
