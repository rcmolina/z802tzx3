/******************************************************************************
**
** wavflip
**                                                                       
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

FILE *fhi, *fho;
long flen, folen;
#define MAXBUFFER	256
unsigned char buf[MAXBUFFER];
long pos;
int ssigned = 0;
int value;

long FileLength (FILE* fh);
void Error (char *errstr);
void ChangeFileBasename (char *str, char *ext);
void writeword(unsigned int i, FILE *fp);
void writestring(char *mystring, FILE *fp);
void writelong(unsigned long i, FILE *fp);

int main (int argc, char *argv[])
{

  if (argc < 2 || argc > 3)
    {
    printf ("\nUsage: wavflip input.wav \n");
    exit (0);
    }

  if (argc == 2) 
    {  
    strcpy (buf, argv[1]); 
    ChangeFileBasename (buf, "_i");
    }
  else      
    strcpy (buf, argv[2]);

  if ((fhi = fopen (argv[1], "rb")) == NULL) 
    Error ("Can't read file!");

  if ((fho = fopen (buf, "wb")) == NULL) 
    Error ("Can't create file!");

  flen = FileLength (fhi);

  fread (buf,1,44,fhi);	   //skip wav file header (44bytes)
  
  if (buf[34] == 16) ssigned=1;
  else if (buf[34] == 8) ssigned=0;
  else {
  	fclose (fhi);
  	fclose (fho);   
  	Error ("Unsupported BitsPerSample value!");
  }

  fwrite (buf,1,44,fho);
  	  
  pos =44;
  int i;
  int blocklength=0;
  while (pos < flen)
  {
  	blocklength = fread(buf, 1, MAXBUFFER, fhi);
	
		if (ssigned) {
			for (i=0;i<blocklength;i+=2) {  
				value = (-(buf[i] + 256*buf[i+1])) & 0xFFFF;
				buf[i]= value % 256;
				buf[i+1]= value / 256;
			}
		}
		
		else {
			for (i=0;i<blocklength;i++) {  
				buf[i] = 255 - buf[i];
  			}
		}

	fwrite (buf,1,blocklength,fho);
	pos=pos + blocklength;  	 
  }
 
  fclose (fhi);
  fclose (fho);  
  //system("pause");
  return (0);
}

/* Changes the File Basename of String *str with *suff */
void ChangeFileBasename (char *str, char *suff)
{
  int n;
  char *ext=".wav";
  
  n = strlen(str); 

  while (str[n] != '.') 
    n--;

  str[n] = 0;
 
  strcat (str, suff); 
  strcat (str, ext); 
}

/* Determine length of file */
long FileLength (FILE* fh)
{
  long curpos, size;
  
  curpos = ftell (fh);
  fseek (fh, 0, SEEK_END);
  size = ftell (fh);
  fseek (fh, curpos, SEEK_SET);
  return (size);
}

/* exits with an error message *errstr */
void Error (char *errstr)
{
  printf ("\n-- Error: %s ('%s')\n", errstr, strerror (errno));
  exit (0);
}

/* Writing routines */
void writebyte(unsigned char c, FILE *fp)
{
        fputc(c,fp);
}


void writeword(unsigned int i, FILE *fp)
{
    fputc(i%256,fp);
    fputc(i/256,fp);
}


void writestring(char *mystring, FILE *fp)
{
    size_t c;

    for (c=0; c < strlen(mystring); c++) {
        writebyte(mystring[c],fp);
    }
}


void writelong(unsigned long i, FILE *fp)
{
    writeword(i%65536,fp);
    writeword(i/65536,fp);
}

