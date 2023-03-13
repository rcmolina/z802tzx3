CC	= gcc
CFLAGS	= -O2 -I/usr/include
ALLFLG	= -DUSE_ALLEGRO
GGIFLG	= -DUSE_GGI
SDLFLG	= -DUSE_SDL
VGAFLG	= -DUSE_SVGA
OGLFLG	= -DUSE_OPENGL
LIBALL	= `allegro-config --libs`
LIBGGI	= -lggi
LIBSDL	= `sdl-config --libs`
LIBVGA	= -lvga
LIBOGL	= -L/usr/X11R6/lib -lglut -lGL -lGLU -lXm

SRC	= main.c

all:
	@echo -e "\n\nPlease use:"
	@echo "   make zxggi     - to build zxscrview using GGI"
	@echo "   make zxall     - to build zxscrview using Allegro"
	@echo "   make zxsdl     - to build zxscrview using SDL"
	@echo "   make zxvga     - to build zxscrview using SVGA"
	@echo "   make zxogl     - to build zxscrview using OpenGL"
	@echo -e "\n\n"


zxggi:
	$(CC) $(CFLAGS) $(GGIFLG) -o zxscrview $(SRC) $(LIBGGI)

zxall:
	$(CC) $(CFLAGS) $(ALLFLG) -o zxscrview $(SRC) $(LIBALL)

zxsdl:
	$(CC) $(CFLAGS) $(SDLFLG) -o zxscrview $(SRC) $(LIBSDL)

zxvga:
	$(CC) $(CFLAGS) $(VGAFLG) -o zxscrview $(SRC) $(LIBVGA)

zxogl:
	$(CC) $(CFLAGS) $(OGLFLG) -o zxscrview $(SRC) $(LIBOGL)

clean:
	rm zxscrview
