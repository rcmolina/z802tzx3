#include <stdio.h>
#include <stdlib.h> //strtol()
#include <fcntl.h>
#include <ctype.h> //toupper()
//#include <unistd.h>
//#include <sys/stat.h>

int main(int argc, char **argv) {

int address=60000;
int hex=1;
int remspec= 0;
int remzx81= 0;
int line= 10;
int inc= 10;
int bytes= 0;
int apos= 0;
int procline, i;

//struct stat file_info;
int fd;
unsigned char c;

if ( argc<2 || argc>4 ) {
	printf("Usage: %s binfile [numline] Dusraddr|Xusraddr|Rusraddrspec|Zusraddrzx81 \n\n",argv[0]);
	return -1;
}
else if (argc==3) {
	if ( argv[2][0] >= 48 && argv[2][0] <= 57) line=strtol(argv[2],NULL,0);
	else if ( argv[2][0] =='-'  && (argv[2][1] >= 48 && argv[2][1] <= 57) ) line=strtol(argv[2],NULL,0);
	else apos= 2;
}
else if (argc==4) {
	if ( argv[2][0] >= 48 && argv[2][0] <= 57) line=strtol(argv[2],NULL,0);
	else if ( argv[2][0] =='-'  && (argv[2][1] >= 48 && argv[2][1] <= 57) ) line=strtol(argv[2],NULL,0);
	else apos= 2;
	if ( argv[3][0] >= 48 && argv[3][0] <= 57) line=strtol(argv[3],NULL,0);
	else if ( argv[3][0] =='-'  && (argv[3][1] >= 48 && argv[3][1] <= 57) ) line=strtol(argv[2],NULL,0);
	else apos= 3;	   
}
if (apos){
	if (toupper(argv[apos][0])=='D') {hex=0;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='X') {hex=1;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='R') {remspec=1;hex=1;address= strtol(argv[apos] +1,NULL,0);}
	else if (toupper(argv[apos][0])=='Z') {remzx81=1;hex=1;address= strtol(argv[apos] +1,NULL,0);}
	else {printf("usraddr type is wrong!\n");return -1;}
}
if (line ==0) line= -1;
if (line <0) {line= -line; inc= 1;}

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
    printf("%d GO TO %d\n", line, line+6*inc);
    line+=inc;
    procline=line;
    printf("%d FOR i=1 TO LEN t$ STEP 2\n", line);
    line+=inc;
    printf("%d LET m=CODE t$(i): LET m=m-48: IF m>9 THEN LET m=m-7\n", line);
    line+=inc;
    printf("%d LET n=CODE t$(i+1): LET n=n-48: IF n>9 THEN LET n=n-7\n", line);
    line+=inc;	    	  	   
    printf("%d POKE %d+j,16*m+n: LET j=j+1: NEXT i\n", line, address-1);
    line+=inc;
    printf("%d RETURN\n", line);
    line+=inc;

    #define MAXBLOCK 80
    unsigned char buffer[MAXBLOCK];
    int j;

    printf("%d LET j=1\n", line);
    line+=inc;	    

/*
    i=0; 
    while (i+MAXBLOCK < bytes) {
       read(fd,&buffer,MAXBLOCK);
       printf("%d LET t$= \"", line);
       for (j=0;j<MAXBLOCK;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       line+=10;		   
       printf("%d GO SUB %d\n", line, procline);
       line+=10;
       i=i+MAXBLOCK;	  	  
    }
    if (i < bytes) {    	
       read(fd,&buffer,bytes-i);
       printf("%d LET t$= \"", line);
       for (j=0;j<bytes-i;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       line+=10;		   
       printf("%d GO SUB %d\n", line, procline);
       line+=10;
       i=bytes;
    }		   	     
*/
    int numbytes;
    i=0; 
    while (i< bytes) {
       numbytes=read(fd,&buffer,MAXBLOCK);
       printf("%d LET t$= \"", line);
       for (j=0;j<numbytes;j++) printf("%02X",buffer[j]);
       printf("\"\n");
       line+=inc;	 	    
       printf("%d GO SUB %d\n", line, procline);
       line+=inc;
       i=i+numbytes;	  	  
    }		 

}
else if ( !remspec && !remzx81 && !hex ) {
    printf("%d RESTORE %d: FOR I=0 TO %d: READ X: POKE %d+I,X:NEXT I\n", line, line+inc, bytes-1, address);
    line+=inc;
    for(i=0;i<bytes;i++) {
       read(fd,&c,1);
       if (i%16==0) {
          printf("%d DATA ",line);
          line+=inc;
       }
       printf("%d",c);
       if ((i%16!=15) && (i!=(bytes-1))) printf(",");
       else printf("\n");
    }
}
else if (remspec) {
    printf("1 REM ");
	if (inc==1) { //dec
	    if (address >= 16384) {
		  printf("\\{33}\\{220}\\{92}"); //HL
		  printf("\\{17}\\{%d}\\{%d}\\{1}\\{%d}\\{%d}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{237}\\{176}\\{201}"); // LDIR, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{%d}",c);
		}
	}
	else { // default inc=10, so use hex
	    if (address >= 16384) {
		  printf("\\{0x21}\\{0xDC}\\{0x5C}"); //HL
		  printf("\\{0x11}\\{0x%02X}\\{0x%02X}\\{0x01}\\{0x%02X}\\{0x%02X}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{0xED}\\{0xB0}\\{0xC9}"); // LDIR, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{0x%02X}",c);
		}
	}	 
	printf("\n");

    if (address >= 16384) {	   
	  printf("2 RANDOMIZE USR VAL \"23760\"\n");
	  printf("3 RANDOMIZE USR VAL \"%d\"\n", address);
	}	  
}
else if (remzx81) {
    printf("1 REM ");
	if (inc==1) { //dec
	    if (address >= 16384) {
		  printf("\\{33}\\{148}\\{64}"); //HL
		  printf("\\{17}\\{%d}\\{%d}\\{1}\\{%d}\\{%d}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{237}\\{176}\\{201}"); // LDIR, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{%d}",c);
		}
	}
	else { // default inc=10, so use hex
	    if (address >= 16384) {
		  printf("\\{0x21}\\{0x94}\\{0x40}"); //HL
		  printf("\\{0x11}\\{0x%02X}\\{0x%02X}\\{0x01}\\{0x%02X}\\{0x%02X}", address%256, address/256, bytes%256, bytes/256); //DE, BC
		  printf("\\{0xED}\\{0xB0}\\{0xC9}"); // LDIR, RET
		}
	    for(i=0;i<bytes;i++) {
	       read(fd,&c,1);
	       printf("\\{0x%02X}",c);
		}
	}	
	printf("\n");

    if (address >= 16384) {	   
	  printf("2 RAND USR VAL \"16514\"\n");
	  printf("3 RAND USR VAL \"%d\"\n", address);
	}	  
}

close(fd);
return 0;
}

