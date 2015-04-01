OPTIONS=-std=c99 -lcurl -lSDL -lSDL_mixer -lSDL_image `pkg-config --cflags gtk+-2.0`
POSTFIX=`pkg-config --libs gtk+-2.0`
all::camera interface

camera::
	gcc shapes.c camera.c -o camera $(POSTFIX) $(OPTIONS)
interface::
	gcc interface.c -o interface $(POSTFIX) $(OPTIONS)
debug::dcamera dinterface

dcamera::
	gcc -g shapes.c camera.c -o camera $(POSTFIX) $(OPTIONS)
dinterface::
	gcc -g interface.c -o interface $(POSTFIX) $(OPTIONS)
