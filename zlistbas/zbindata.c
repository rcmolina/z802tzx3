#include <stdio.h>
#include <stdlib.h> //strtol()
#include <fcntl.h>
#include <ctype.h> //toupper()
//#include <unistd.h>
//#include <sys/stat.h>

#define PROG_VER "1.3"

int main(int argc, char **argv) {

int address= 60000;
int hex= 1;
int loadrfirst= 0;
int remspec= 0;
int remzx81= 0;
int numline= 10;
int inc= 10;
int bytes= 0;
int apos= 0;
int procline, i;
int rembase= 0;

//struct stat file_info;
int fd;
unsigned char c;

if ( argc<2 || argc>4 ) {
	printf("v.%s\n", PROG_VER); 
	printf("Usage: %s binfile [[-]numline] Dusraddr    |Xusraddr    |\n",argv[0]);
	printf("                                     Rusraddrspec|Zusraddrzx81\n");
	return -1;
}
else if (argc==3) {
	if ( argv[2][0] >= 48 && argv[2][0] <= 57) numline=strtol(argv[2],NULL,0);
	else if ( argv[2][0] =='-'  && (argv[2][1] >= 48 && argv[2][1] <= 57) ) numline=strtol(argv[2],NULL,0);
	else apos= 2;
}
else if (argc==4) {
	if ( argv[2][0] >= 48 && argv[2][0] <= 57) numline=strtol(argv[2],NULL,0);
	else if ( argv[2][0] =='-'  && (argv[2][1] >= 48 && argv[2][1] <= 57) ) numline=strtol(argv[2],NULL,0);
	else apos= 2;
	if ( argv[3][0] >= 48 && argv[3][0] <= 57) numline=strtol(argv[3],NULL,0);
	else if ( argv[3][0] =='-'  && (argv[3][1] >= 48 && argv[3][1] <= 57) ) numline=strtol(argv[2],NULL,0);
	else apos= 3;	   
}
if (apos){
	if (toupper(argv[apos][0])=='D') {hex=0;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='X') {hex=1;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='R') {remspec=1;hex=1;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='Z') {remzx81=1;hex=1;address= strtol(argv[apos] +1,NULL,0);}  
	else {printf("usraddr type is wrong!\n");return -1;}
}
if (numline ==0) numline= -1;
if (numline <0) {numline= -numline; inc= 1;}

/*
if (stat(argv[1],&file_info)<0) {
	fprintf(stderr,"Could not stat file %s\n\n",argv[1]);
	return -1;
}
bytes=(int)file_info.st_size;
*/

fd=open(argv[1],O_RDONLY|O_BINARY);
if (fd<0) {
	fprintf(stderr,"Could not open file %s\n\n",argv[1]);
	return -1;
}
lseek (fd, 0, SEEK_END);
bytes=(int)tell(fd);
lseek (fd, 0, SEEK_SET);

if ( !remspec && !remzx81 && hex ) {
    printf("%d GO TO %d\n", numline, numline+6*inc);
    numline+=inc;
    procline=numline;
    printf("%d FOR i=1 TO LEN t$ STEP 2\n", numline);
    numline+=inc;
    printf("%d LET m=CODE t$(i): LET m=m-48: IF m>9 THEN LET m=m-7\n", numline);
    numline+=inc;
    printf("%d LET n=CODE t$(i+1): LET n=n-48: IF n>9 THEN LET n=n-7\n", numline);
    numline+=inc;	   	     	  
    printf("%d POKE %d+j,16*m+n: LET j=j+1: NEXT i\n", numline, address-1);
    numline+=inc;
    printf("%d RETURN\n", numline);
    numline+=inc;

    #define MAXBLOCK 80
    unsigned char buffer[MAXBLOCK];
    int j;

    printf("%d LET j=1\n", numline);
    numline+=inc;	   

/*
    i=0; 
    while (i+MAXBLOCK < bytes) {
       read(fd,&buffer,MAXBLOCK);
       printf("%d LET t$= \"", numline);
       for (j=0;j<MAXBLOCK;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       numline+=10;	   	      
       printf("%d GO SUB %d\n", numline, procline);
       numline+=10;
       i=i+MAXBLOCK;	  	  
    }
    if (i < bytes) {    	
       read(fd,&buffer,bytes-i);
       printf("%d LET t$= \"", numline);
       for (j=0;j<bytes-i;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       numline+=10;	   	      
       printf("%d GO SUB %d\n", numline, procline);
       numline+=10;
       i=bytes;
    }		   	     
*/
    int numbytes;
    i=0; 
    while (i< bytes) {
       numbytes=read(fd,&buffer,MAXBLOCK);
       printf("%d LET t$= \"", numline);
       for (j=0;j<numbytes;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       numline+=inc;		   
       printf("%d GO SUB %d\n", numline, procline);
       numline+=inc;
       i=i+numbytes;	  	  
    }		 

}
else if ( !remspec && !remzx81 && !hex ) {
    printf("%d RESTORE %d: FOR I=0 TO %d: READ X: POKE %d+I,X:NEXT I\n", numline, numline+inc, bytes-1, address);
    numline+=inc;
    for(i=0;i<bytes;i++) {
       read(fd,&c,1);
       if (i%16==0) {
          printf("%d DATA ",numline);
          numline+=inc;
       }
       printf("%d",c);
       if ((i%16!=15) && (i!=(bytes-1))) printf(",");
       else printf("\n");
    }
}

else if (remspec || remzx81) {
	if (remspec) rembase= 23760;
	if (remzx81) rembase= 16514;

	int loadrsize= 17;
	char randcmd[]= "RANDOMIZE";
    if (numline <16384) numline= address;
    printf("1 REM ");
	if (inc==1) { //dec
	    if (address >= rembase) {
		  printf("\\{243}\\{33}\\{%d}\\{%d}", (rembase +loadrsize +bytes -1)%256, (rembase +loadrsize +bytes -1)/256); //DI, HL
		  printf("\\{17}\\{%d}\\{%d}\\{1}\\{%d}\\{%d}", (address +bytes -1)%256, (address +bytes -1)/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{237}\\{184}\\{251}\\{195}\\{%d}\\{%d}\\{201}", numline%256, numline/256); // LDDR, EI, JP numline, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{%d}",c);
		}
	    if (address >= 16384 && address < rembase) {
		  printf("\\{243}\\{33}\\{%d}\\{%d}", rembase%256, rembase/256); //DI, HL rembase
		  printf("\\{17}\\{%d}\\{%d}\\{1}\\{%d}\\{%d}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{237}\\{176}\\{251}\\{195}\\{%d}\\{%d}\\{201}", numline%256, numline/256);  // LDIR, EI, JP numline, RET
		}
	}
	else { // default inc=10, so use hex
	    if (address >= rembase) {
		  printf("\\{F3}\\{0x21}\\{0x%02X}\\{0x%02X}", (rembase +loadrsize +bytes -1)%256, (rembase +loadrsize +bytes -1)/256); //HL
		  printf("\\{0x11}\\{0x%02X}\\{0x%02X}\\{0x01}\\{0x%02X}\\{0x%02X}", (address +bytes -1)%256, (address +bytes -1)/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{0xED}\\{0xB8}\\{0xFB}\\{0xC3}\\{0x%02X}\\{0x%02X}\\{0xC9}", numline%256, numline/256); // LDDR, EI, JP numline, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{0x%02X}",c);
		}

	    if (address >= 16384 && address < rembase) {
		  printf("\\{0xF3}\\{0x21}\\{0x%02X}\\{0x%02X}", rembase%256, rembase/256); //DI, HL rembase
		  printf("\\{0x11}\\{0x%02X}\\{0x%02X}\\{0x01}\\{0x%02X}\\{0x%02X}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{0xED}\\{0xB0}\\{0xFB}\\{0xC3}\\{0x%02X}\\{0x%02X}\\{0xC9}", numline%256, numline/256); // LDIR, EI, JP numline, RET
		}
	}	 
	printf("\n");
	if (remzx81) randcmd[4]=0; //RAND instead of RANDOMIZE
	
	if (address >= rembase) {
		printf("2 %s USR VAL \"%d\"\n", randcmd, rembase); 
		printf("3 %s USR VAL \"%d\"\n", randcmd, numline);
	}
	else if (address >= 16384 && address < rembase) {
		printf("2 %s USR VAL \"%d\"\n", randcmd, rembase +bytes); 
		printf("3 %s USR VAL \"%d\"\n", randcmd, numline);
	}
	else if (numline >= 16384) printf("2 %s USR VAL \"%d\"\n", randcmd, numline);
	else printf("2 %s USR VAL \"%d\"\n", randcmd, rembase);
}


close(fd);
return 0;

}




