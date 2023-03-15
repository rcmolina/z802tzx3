// ZX-Spectrum SCREEN$ viewer
// (c) 2003 Catalin Mihaila <catalin@idgrup.ro>

//#include <iostream>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>


#define USE_OPENGL

#ifdef USE_OPENGL
#include <GL/glut.h>
#endif

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_GGI
#include <ggi/ggi.h>
#endif

#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#ifdef USE_SVGA
#include <vga.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#define MAJV 0
#define MINV 2
#define CAPTION "SCREEN$ viewer V%d.%d"

#ifdef USE_SDL
SDL_Surface *scrbuf;
SDL_Event event;
#endif

#ifdef USE_GGI
ggi_visual_t vis;
ggi_mode mo;
ggi_color col;
#endif

#ifdef USE_ALLEGRO
static BITMAP *scrbuf = NULL;
#endif

#define SPSCR argv[1]

#define SCREENWIDTH 256
#define SCREENHEIGHT 192

char zxscr[6144];
char attrs[768];

#ifdef USE_OPENGL
float colour_palette[] = {
    0,0,0,
    0,0,0.75,
    0.75,0,0,
    0.75,0,0.75,
    0,0.75,0,
    0,0.75,0.75,
    0.75,0.75,0,
    0.75,0.75,0.75,
    0,0,0,
    0,0,1,
    1,0,0,
    1,0,1,
    0,1,0,
    0,1,1,
    1,1,0,
    1,1,1
#else
unsigned int colour_palette[] = {
#ifdef USE_GGI
    0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xC000,
    0xC000, 0x0000, 0x0000,
    0xC000, 0x0000, 0xC000,
    0x0000, 0xC000, 0x0000,
    0x0000, 0xC000, 0xC000,
    0xC000, 0xC000, 0x0000,
    0xC000, 0xC000, 0xC000,
    0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFFFF,
    0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000,
    0x0000, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF
#else
    0,0,0,
    0,0,192,
    192,0,0,
    192,0,192,
    0,192,0,
    0,192,192,
    192,192,0,
    192,192,192,
    0,0,0,
    0,0,255,
    255,0,0,
    255,0,255,
    0,255,0,
    0,255,255,
    255,255,0,
    255,255,255
#endif
#endif
};

int draw_screen(void);

#ifdef USE_OPENGL
void gldisplay (void)
{

    int i;

    glClear( GL_COLOR_BUFFER_BIT );
    glBegin(GL_POINTS);
    draw_screen();

// comento arriba y activo prueba
/*
    for(i=0;i<SCREENWIDTH;i++) {
	    glColor3f(colour_palette[2], colour_palette[5], colour_palette[12]);
	    //glVertex2i(i, SCREENHEIGHT-10);
		//glVertex3f(-1+(float)i*2/SCREENWIDTH,0,0); // 0=middle point
		glVertex2f(0,-1+(float)i*2/SCREENHEIGHT);	   	   
    }
	
*/
//cont
    glEnd();
    glFinish();


}

static void reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glFlush();
}

void OnWindowResize(int width, int height)
{
    //See how the output changes if you change left and top
    int left = 0;
    int bottom = 0;

    glViewport(left, bottom, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}


void OnKeyPress(unsigned char key, int x, int y)
{
# define ESC_Key 27

    switch (key)
    {
        case ESC_Key:
            exit(0);
        break;
    }
}

void Draw()
{
/*
    glClearColor(0.1f, 0.1f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

        glColor3f(0.2f, 0.2f, 0.9f);

        glVertex3f(-1.0f, -1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
        glVertex3f( 0.0f,  1.0f, 0.0f);

    glEnd();

    glFlush();
*/

    //glClearColor(0.1f, 0.1f, 0.5f, 0.0f);	  // pantalla azul, por defecto negra
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);			// color del punto white

    glPointSize(2.0f);    
	glBegin(GL_POINTS); //starts drawing of points (x,y,z)
      //glVertex3f(1.0f,1.0f,0.0f);//upper-right corner point
      //glVertex3f(-1.0f,-1.0f,0.0f);//lower-left corner point

    float x;
   
    for(x = 0; x < 200; x++)
	   	glVertex3f(-1+(float)x/100,-1+(float)x/100,0.0f); // middle point


    glEnd();//end drawing of points

	glutSwapBuffers();

}


#endif
								

int main (int argc, char *argv[])
{
    int i,d,q;
    int mainloop=1;
    int fisier;
    int octet;
#ifdef USE_OPENGL
    char windowtitle[256];
    sprintf(windowtitle,CAPTION,MAJV,MINV);
#endif


  char filename[ MAX_PATH ];

  OPENFILENAME ofn;
    ZeroMemory( &filename, sizeof( filename ) );
    ZeroMemory( &ofn,      sizeof( ofn ) );
    ofn.lStructSize  = sizeof( ofn );
    ofn.hwndOwner    = NULL;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter  = "SCR Files\0*.scr\0Any File\0*.*\0";
    ofn.lpstrFile    = filename;
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrTitle   = "Select a File to view!";
//    ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
  
  if (!(GetOpenFileNameA( &ofn )))
  {
    // All this stuff below is to tell you exactly how you messed up above. 
    // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
    switch (CommDlgExtendedError())
    {
	/*
      case CDERR_DIALOGFAILURE   : std::cout << "CDERR_DIALOGFAILURE\n";   break;
      case CDERR_FINDRESFAILURE  : std::cout << "CDERR_FINDRESFAILURE\n";  break;
      case CDERR_INITIALIZATION  : std::cout << "CDERR_INITIALIZATION\n";  break;
      case CDERR_LOADRESFAILURE  : std::cout << "CDERR_LOADRESFAILURE\n";  break;
      case CDERR_LOADSTRFAILURE  : std::cout << "CDERR_LOADSTRFAILURE\n";  break;
      case CDERR_LOCKRESFAILURE  : std::cout << "CDERR_LOCKRESFAILURE\n";  break;
      case CDERR_MEMALLOCFAILURE : std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
      case CDERR_MEMLOCKFAILURE  : std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
      case CDERR_NOHINSTANCE     : std::cout << "CDERR_NOHINSTANCE\n";     break;
      case CDERR_NOHOOK          : std::cout << "CDERR_NOHOOK\n";          break;
      case CDERR_NOTEMPLATE      : std::cout << "CDERR_NOTEMPLATE\n";      break;
      case CDERR_STRUCTSIZE      : std::cout << "CDERR_STRUCTSIZE\n";      break;
      case FNERR_BUFFERTOOSMALL  : std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
      case FNERR_INVALIDFILENAME : std::cout << "FNERR_INVALIDFILENAME\n"; break;
      case FNERR_SUBCLASSFAILURE : std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
      default                    : std::cout << "You cancelled.\n";
*/	    

      case CDERR_DIALOGFAILURE   : printf("CDERR_DIALOGFAILURE\n");   break;
      case CDERR_FINDRESFAILURE  : printf("CDERR_FINDRESFAILURE\n");  break;
      case CDERR_INITIALIZATION  : printf("CDERR_INITIALIZATION\n");  break;
      case CDERR_LOADRESFAILURE  : printf("CDERR_LOADRESFAILURE\n");  break;
      case CDERR_LOADSTRFAILURE  : printf("CDERR_LOADSTRFAILURE\n");  break;
      case CDERR_LOCKRESFAILURE  : printf("CDERR_LOCKRESFAILURE\n");  break;
      case CDERR_MEMALLOCFAILURE : printf("CDERR_MEMALLOCFAILURE\n"); break;
      case CDERR_MEMLOCKFAILURE  : printf("CDERR_MEMLOCKFAILURE\n");  break;
      case CDERR_NOHINSTANCE     : printf("CDERR_NOHINSTANCE\n");     break;
      case CDERR_NOHOOK          : printf("CDERR_NOHOOK\n");          break;
      case CDERR_NOTEMPLATE      : printf("CDERR_NOTEMPLATE\n");      break;
      case CDERR_STRUCTSIZE      : printf("CDERR_STRUCTSIZE\n");      break;
      case FNERR_BUFFERTOOSMALL  : printf("FNERR_BUFFERTOOSMALL\n");  break;
      case FNERR_INVALIDFILENAME : printf("FNERR_INVALIDFILENAME\n"); break;
      case FNERR_SUBCLASSFAILURE : printf("FNERR_SUBCLASSFAILURE\n"); break;
      default                    : printf("You cancelled.\n");	      
	  
	  
    }
  }

    //fisier = open(SPSCR,O_RDONLY);
	fisier = open(filename,O_RDONLY);
    if(fisier < 0) { printf("No SCREEN$ file given.\n"); return 1;}
	
    read(fisier,zxscr,6144);
	lseek(fisier,6144,SEEK_SET);
    read(fisier,attrs,768);
    close(fisier);	    

#ifdef USE_OPENGL
/*
    glutInit(&argc,argv);
    glutInitWindowSize( SCREENWIDTH, SCREENHEIGHT );	
    glutInitWindowPosition( 0, 0 );
    glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );	  
    glutCreateWindow(windowtitle);
    glutDisplayFunc( gldisplay );
    glutReshapeFunc( reshape );
    glutMainLoop();
*/

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT); 
    glutInitWindowPosition(0, 0);
    glutCreateWindow(windowtitle);
    
    //glutDisplayFunc(Draw);
	glutDisplayFunc( gldisplay );
    glutReshapeFunc(OnWindowResize);
    glutKeyboardFunc(OnKeyPress);


    glutMainLoop();	   
	
#else

    if(init_graphic_lib()) { printf("ERROR\n"); return 1; }

    if(init_graphic_window()) { printf("ERROR\n"); return 1; }

    draw_screen();
    update_screen();

    while(mainloop){mainloop=readkeys();}
    
    close_graphic_lib();
#endif
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN();
#endif

int draw_screen_pixels (int octet_val, int attr_nr, int pix, int piy);

int draw_screen(void)
{

    int octetnr;
    int d,i,k;

    for(k=0;k<3;k++)
	{
	    for(d=0;d<8;d++)
		{
		    for(i=0;i<8;i++)
			{
			    for(octetnr=(d*32);octetnr<((d*32)+32);octetnr++)
				{
				
				    draw_screen_pixels(zxscr[octetnr+(256*i)+(k*2048)], attrs[octetnr+(256*k)],(((8*octetnr)-(256*d))
#ifdef USE_SVGA
				    +32
#endif
				    ),
				    ((i+(8*d)+(k*64))
#ifdef USE_SVGA
				    +24
#endif
				    ));

					//printf("%d %d %d octect number: %d zxscr:%02X attrs:%02X\n",k,d,i,octetnr,(unsigned char)zxscr[octetnr+(256*i)+(k*2048)], (unsigned char)attrs[octetnr+(256*k)] ); 
					//system("pause");
					//draw_screen_pixels( zxscr[octetnr+(256*i)],attrs[octetnr], (8*octetnr), i  );					
				//	  draw_screen_pixels( zxscr[octetnr+(256*i)+(k*2048)],attrs[octetnr+(256*k)], (8*octetnr)-(256*d), i+(8*d)+(k*64)  );
				}
			}
		}
	}



    //float x;   
    //for(x = 0; x < 200; x++)
	   	//glVertex3f(-1+(float)x/100,-1+(float)x/100,0.0f); // 0=middle point

//	  glVertex3f(0,0,0.0f); // 0=middle point


/*
	int i;
    for(i=0;i<SCREENWIDTH;i++) {
	    glColor3f(colour_palette[2], colour_palette[5], colour_palette[12]);
	    //glVertex2i(i, SCREENHEIGHT-10);
		//glVertex3f(-1+(float)i*2/SCREENWIDTH,0,0); // 0=middle point
		glVertex2f(0,-1+(float)i*2/SCREENHEIGHT);	   	   
    }	     
*/	    

/*
	k=d=i=0;
	for(octetnr=0;octetnr<32;octetnr++){
		printf("%d %d %d octect number: %d zxscr:%02X attrs:%02X\n",k,d,i,octetnr,(unsigned char)zxscr[octetnr+(256*i)+(k*2048)], (unsigned char)attrs[octetnr+(256*k)] ); 
		system("pause");
		draw_screen_pixels( zxscr[octetnr],attrs[octetnr], 8*octetnr, 0  );	  	  
	}
*/
/*
		k=d=i=octetnr=0;
		printf("%d %d %d octect number: %d zxscr:%02X attrs:%02X\n",k,d,i,octetnr,(unsigned char)zxscr[octetnr+(256*i)+(k*2048)], (unsigned char)attrs[octetnr+(256*k)] ); 
		system("pause");
		//draw_screen_pixels( zxscr[octetnr],attrs[octetnr], 8*octetnr, 0  );
		//draw_screen_pixels( 126,0, 0, 0  );	  	  	  ; aqui parece que esta el problema
		//glColor3f(colour_palette[3*2], colour_palette[1+3*2], colour_palette[2+3*2]);
		//glColor3f(colour_palette[3*6], colour_palette[1+3*6], colour_palette[2+3*6]);
		glVertex2i(0, SCREENHEIGHT);
*/
    return 0;
  

}	 


#ifndef USE_OPENGL
int init_graphic_lib (void)
{
    if(
#ifdef USE_SDL
    SDL_Init( SDL_INIT_VIDEO )
#endif
#ifdef USE_GGI
    ggiInit()
#endif
#ifdef USE_ALLEGRO
    allegro_init()
#endif
#ifdef USE_SVGA
    vga_init()
#endif
    < 0 )
	{
	    return 1;
	} else {
	    return 0;
	}
}
#endif

#ifndef USE_OPENGL
int init_graphic_window (void)
{
#ifdef USE_SVGA
    int i;
#else
#ifndef USE_GGI
    char windowtitle[256];
    sprintf(windowtitle,CAPTION,MAJV,MINV);
#endif
#endif

#ifdef USE_SDL
    scrbuf = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, 16, SDL_SWSURFACE|SDL_ANYFORMAT );
    if( scrbuf == NULL )
	{
	    return(1);
	}
    SDL_WM_SetCaption( windowtitle, "" );
#endif

#ifdef USE_GGI
    vis=ggiOpen(NULL);

    if(vis == NULL)
        {
            printf("unable to open default visual.\n");
            return 1;
        }

    ggiSetFlags(vis, GGIFLAG_ASYNC);

    mo.virt.x = SCREENWIDTH;
    mo.virt.y = SCREENHEIGHT;

    mo.visible.x = GGI_AUTO;
    mo.visible.y = GGI_AUTO;

    mo.frames = GGI_AUTO;
    mo.graphtype = GGI_AUTO;

    mo.dpp.x = 1;
    mo.dpp.y = 1;

    if(ggiCheckMode(vis,&mo) < 0) {return 1;}
    if(ggiSetMode(vis,&mo) < 0) {return 1;}
#endif

#ifdef USE_ALLEGRO
    install_keyboard();
    set_window_title(windowtitle);
    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT, SCREENWIDTH, SCREENHEIGHT, 0, 0);
    set_pallete(desktop_pallete);
    scrbuf = create_bitmap(SCREENWIDTH, SCREENHEIGHT);
#endif

#ifdef USE_SVGA
    keyboard_init();
    vga_setmode(G320x240x256);
    for(i=0;i<16;i++)
	{
	    vga_setpalette(i,colour_palette[i*3]>>2,colour_palette[i*3+1]>>2, colour_palette[i*3+2]>>2);
	}
#endif
    return 0;
}
#endif

#ifndef USE_OPENGL
int readkeys (void)
{
#ifdef USE_SDL
    while( SDL_PollEvent(&event))
	{
	    switch( event.type )
		{
		    case SDL_QUIT:
		    case SDL_KEYDOWN:
			return 0;
		    break;

		    default:
		    break;
		}
	}
#endif

#ifdef USE_GGI
    ggi_event ev;
    struct timeval t = {0,0};
    ggi_event_mask mask;
    mask = ggiEventPoll(vis, emAll, &t);
    if (!mask) { return 1; }

    while (ggiEventsQueued(vis, mask))
        {
            ggiEventRead(vis, &ev, mask);
            switch (ev.any.type)
                {
                    case evKeyPress:
                        return 0;
                    break;

                    default:
                    break;
                }
        }
#endif

#ifdef USE_ALLEGRO
    while(keypressed())
        {
            if(readkey()) { return 0; }
        }
#endif

#ifdef USE_SVGA
    vga_waitevent(VGA_KEYEVENT, NULL, NULL, NULL, NULL);
    return 0;
#else
    return 1;
#endif
}
#endif

int draw_screen_pixels (int octet_val, int attr_nr, int pix, int piy)
{

    int binar[8];
    int attrbin[8];
    int *pbin;
    int *abin;
    int i;
    int d = 1;
    int ink,paper,bright;
#ifndef USE_OPENGL
#ifdef USE_SDL
    Uint32 px;
#else
#ifndef USE_GGI
    int px;
#endif
#endif
#ifdef USE_SVGA
    unsigned char linebuf[1];
#endif
#endif
	  
	
    pbin = binar;
    abin = attrbin;

    for(i=0;i<8;i++)
	{
            if(octet_val&d)
                {
                    *pbin = 1;
                } else {
                    *pbin = 0;
                }
            d=d*2;
            pbin++;
        }

    d = 1;

    for(i=0;i<8;i++)
	{
            if(attr_nr&d)
                {
                    *abin = 1;
                } else {
                    *abin = 0;
                }
            d=d*2;
            abin++;
        }

    ink = (attrbin[0]+(2*attrbin[1])+(4*attrbin[2]));
    paper = (attrbin[3]+(2*attrbin[4])+(4*attrbin[5]));
    bright = attrbin[6];

    if(bright) { ink=ink+8; paper=paper+8; }



    for(i=7;i>-1;i--)
        {
	    if(binar[i])
		{
#ifdef USE_OPENGL
		    glColor3f(colour_palette[3*ink], colour_palette[1+3*ink], colour_palette[2+3*ink]);
#else
#ifdef USE_SDL
		    px=SDL_MapRGB(scrbuf->format, colour_palette[3*ink], colour_palette[1+3*ink], colour_palette[2+3*ink]);
#endif

#ifdef USE_GGI
                    col.r = colour_palette[3*ink];
                    col.g = colour_palette[1+3*ink];
                    col.b = colour_palette[2+3*ink];
#endif

#ifdef USE_ALLEGRO
                    px = makecol(colour_palette[3*ink], colour_palette[(1+(3*ink))], colour_palette[(2+(3*ink))]);
#endif

#ifdef USE_SVGA
		    px = ink;
#endif
#endif

		} else {

#ifdef USE_OPENGL
		    glColor3f(colour_palette[3*paper], colour_palette[1+3*paper], colour_palette[2+3*paper]);
#else
#ifdef USE_SDL
		    px=SDL_MapRGB(scrbuf->format, colour_palette[3*paper], colour_palette[1+3*paper], colour_palette[2+3*paper]);
#endif

#ifdef USE_GGI
		    col.r = colour_palette[3*paper];
                    col.g = colour_palette[1+3*paper];
		    col.b = colour_palette[2+3*paper];
#endif

#ifdef USE_ALLEGRO
		    px = makecol(colour_palette[3*paper], colour_palette[(1+(3*paper))], colour_palette[(2+(3*paper))]);
#endif

#ifdef USE_SVGA
		    px = paper;
#endif
#endif
		}

#ifdef USE_OPENGL
	    //glVertex2i(pix, SCREENHEIGHT-piy);
		glVertex2f( ((float)pix*2/SCREENWIDTH-1), 1-(float)piy*2/SCREENHEIGHT );
#else
#ifdef USE_GGI
            ggiSetGCForeground(vis,ggiMapColor(vis, &col));
	    ggiDrawPixel(vis,pix,piy);
#else
#ifndef USE_SVGA
	    putpixel(scrbuf,pix,piy,px);
#endif
#endif

#ifdef USE_SVGA
	    linebuf[0] = px;
	    vga_drawscansegment(linebuf,pix,piy,1);
#endif
#endif
	    pix++;
        }
		

/*
    for(i=0;i<SCREENWIDTH;i++) {
	    glColor3f(colour_palette[2], colour_palette[5], colour_palette[12]);
	    //glVertex2i(i, SCREENHEIGHT-10);
		//glVertex3f(-1+(float)i*2/SCREENWIDTH,0,0); // 0=middle point
		glVertex2f(0,-1+(float)i*2/SCREENHEIGHT);	   	   
    }
*/

/*
	//printf("%02X %02X ",(unsigned char) octet_val, (unsigned char) attr_nr); 
	//system("pause");	     
    for(i=7;i>-1;i--) {

	    if(binar[i]) glColor3f(colour_palette[3*ink], colour_palette[1+3*ink], colour_palette[2+3*ink]);
		else glColor3f(colour_palette[3*paper], colour_palette[1+3*paper], colour_palette[2+3*paper]);

		
		// black and white	  	  
		//if(binar[i]) glColor3f(255, 255, 255); 	 white ink
		//else glColor3f(0, 0, 0);	  	  	  	  // black paper


		//glColor3f(colour_palette[2], colour_palette[5], colour_palette[12]); //
		//glColor3f(0, 255, 0); 	//RGB
		    
	   	//glVertex2i(pix, SCREENHEIGHT-piy);
		//glVertex2f((float)pix/SCREENWIDTH, (float)(SCREENHEIGHT-piy)*2/SCREENHEIGHT);
		glVertex2f( ((float)pix*2/SCREENWIDTH-1), 1-(float)piy*2/SCREENHEIGHT );	 	  
	    pix++;	  	  	 	 	 

    }
*/ 	   
    return 0;
}



#ifndef USE_OPENGL
int update_screen (void)
{
#ifdef USE_SDL
    SDL_UpdateRect(scrbuf,0,0,0,0);
#endif

#ifdef USE_GGI
    ggiFlush(vis);
#endif

#ifdef USE_ALLEGRO
    blit(scrbuf, screen, 0, 0, 0, 0, scrbuf->w, scrbuf->h);
#endif
    return 0;
}
#endif


#ifndef USE_OPENGL
int close_graphic_lib (void)
{
#ifdef USE_SDL
    SDL_Quit();
#endif

#ifdef USE_GGI
    ggiClose(vis);
    ggiExit();
#endif

#ifdef USE_ALLEGRO
    destroy_bitmap(scrbuf);
    scrbuf = NULL;
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_exit();
#endif

#ifdef USE_SVGA
    vga_setmode( TEXT );
#endif
    return 0;
}
#endif

#ifdef USE_SDL
int putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
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
    return 0;
}
#endif

