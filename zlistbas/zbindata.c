#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char **argv) {

	int address=60000;
	int hex=1;
	int bytes=0,line=10, procline,i;
	struct stat file_info;
	int fd;
	unsigned char c;

	if ((argc<2) || (argc>4)) {
		printf("Usage:\t%s binfile [addr] [dnumline|xnumline] \n\n",argv[0]);
		return -1;
	}

	else if (argc==3) {
		    if (toupper(argv[2][0])=='D') {hex=0;line= strtol(argv[2] +1,NULL,0);}
			else if (toupper(argv[2][0])=='X') {hex=1;line= strtol(argv[2] +1,NULL,0);}
			else address=strtol(argv[2],NULL,0);
	}
	else if (argc==4) {
		    if (toupper(argv[2][0])=='D') {hex=0;line= strtol(argv[2] +1,NULL,0);}
			else if (toupper(argv[2][0])=='X') {hex=1;line= strtol(argv[2] +1,NULL,0);}
			else address=strtol(argv[2],NULL,0);	   

			if (toupper(argv[3][0])=='D') {hex=0;line= strtol(argv[3] +1,NULL,0);}
			else if (toupper(argv[3][0])=='X') {hex=1;line= strtol(argv[3] +1,NULL,0);}
			else address=strtol(argv[3],NULL,0);
	}

	if (stat(argv[1],&file_info)<0) {
		fprintf(stderr,"Could not stat file %s\n\n",argv[1]);
		return -1;
	}
	bytes=(int)file_info.st_size;

	fd=open(argv[1],O_RDONLY|O_BINARY);
	if (fd<0) {
		fprintf(stderr,"Could not open file %s\n\n",argv[1]);
		return -1;
	}

if (hex) {
    printf("%d GO TO %d\n", line, line+60);
    line+=10;
    procline=line;
    printf("%d FOR i=1 TO LEN t$ STEP 2\n", line);
    line+=10;
    printf("%d LET m=CODE t$(i): LET m=m-48: IF m>9 THEN LET m=m-7\n", line);
    line+=10;
    printf("%d LET n=CODE t$(i+1): LET n=n-48: IF n>9 THEN LET n=n-7\n", line);
    line+=10;	   	     	  
    printf("%d POKE %d+j,16*m+n: LET j=j+1: NEXT i\n", line, address-1);
    line+=10;
    printf("%d RETURN\n", line);
    line+=10;

    #define MAXBLOCK 80
    unsigned char buffer[MAXBLOCK];
    int j;

    printf("%d LET j=1\n", line);
    line+=10;	   

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
       line+=10;		   
       printf("%d GO SUB %d\n", line, procline);
       line+=10;
       i=i+numbytes;	  	  
    }		 

}
else {
    printf("%d RESTORE %d: FOR I=0 TO %d: READ X: POKE %d+I,X:NEXT I\n", line, line+10, bytes-1, address);
    line+=10;
    for(i=0;i<bytes;i++) {
       read(fd,&c,1);
       if (i%16==0) {
          printf("%d DATA ",line);
          line+=10;
       }
       printf("%d",c);
       if ((i%16!=15) && (i!=(bytes-1))) printf(",");
       else printf("\n");
    }
}

close(fd);
return 0;
}

