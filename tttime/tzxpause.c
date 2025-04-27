/******************************************************************************
**
** tzxpause
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

#define MAJREV 1        /* Major revision of the format this program supports */
#define MINREV 13        /* Minor revision -||- */

FILE *fhi, *fho;
long flen;
unsigned char *mem;
char buf[256];
long pos;
int len;
int block;
int longer, custom, only, dataonly, direct, not_rec;
unsigned char tzxbuf[10] = {'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1A, 1, 0};

int Get2 (unsigned char *mem) {return (mem[0] + (mem[1] * 256));}
int Get3 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256));}
int Get4 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256) + (mem[3] * 256 * 256 * 256));}

int Put2 (unsigned int value, unsigned char *str) {memset(str, value%256, 1); memset(str+1, value/256, 1); return 0;}


long FileLength (FILE* fh);
void Error (char *errstr);
void ChangeFileBasename (char *str, char *ext);
int pause = 0;

int getnumber(char *s)
{
// Returns the INT number contained in string *s
int i;

sscanf(s,"%d",&i); return(i);
}

int main (int argc, char *argv[])
{
  //printf ("\n - Tape Total Time duration\n");

  if (argc < 3 || argc > 4)
    {
    printf ("\nUsage: tzxpause pausems input.tzx\n");
    exit (0);
    }

  if (argc == 3) 
    {  
    strcpy (buf, argv[2]); 
    ChangeFileBasename (buf, "_fix");
    }
  else      
    strcpy (buf, argv[3]);

  pause = getnumber(argv[1]);
  
  if ((fhi = fopen (argv[2], "rb")) == NULL) 
    Error ("Can't read file!");

  if ((fho = fopen (buf, "wb")) == NULL) 
    Error ("Can't write file!");
	
  flen = FileLength (fhi);

  if ((mem = (unsigned char *) malloc (flen)) == NULL) 
    Error ("Not enough memory to load input file!");

  if (fread (mem, 1, 10, fhi) != 10)
    Error ("Read error!");

  if (fwrite (mem, 1, 10, fho) != 10)
    Error ("Write error!");

  //printf("Tape input filename: %s\n",argv[1]);
  mem[7] = 0;

  if (strcmp ((char *)mem, "ZXTape!")) { 
    free (mem);
    Error ("File is not in ZXTape format!");
  }
  else {
	
	  printf ("\nZXTape file revision %d.%02d\n", mem[8], mem[9]);
	
	  if (!mem[8]) 
	    Error ("Development versions of ZXTape format are not supported!");
	
	  if (mem[8] > MAJREV) 
	    printf ("\n-- Warning: Some blocks may not be recognised and used!\n");
	
	  if (mem[8] == MAJREV && mem[9] > MINREV) 
	    printf ("\n-- Warning: Some of the data might not be properly recognised!\n");
	
	  if (fread (mem, 1, flen - 10, fhi) != (flen - 10))
	    Error ("Read error!");
  }

  printf ("\n");
  pos = block = longer = custom = only = dataonly = direct = not_rec;
  
  while (pos < flen - 10)
    {
    pos++;
    switch (mem[pos - 1])
      {
      case 0x10: 	 	 	  	  	  	  	  	    			 
	  			 len = Get2(&mem[pos + 0x02]);
				 Put2(pause, &mem[pos]);					 
                 pos += len + 0x04;
                 block++;
                 break;
 
      case 0x11: 	 	  
				 len = Get3 (&mem[pos + 0x0F]);
				 Put2(pause, &mem[pos + 0x0D]); 
                 pos += len + 0x12;
                 break;
				 
      case 0x12: 
                 pos += 0x04;
                 break;
      case 0x13: 
                 pos += (mem[pos + 0x00] * 0x02) + 0x01;
                 break;
      case 0x14: 
				 len = Get3 (&mem[pos + 0x07]);
				 Put2(pause, &mem[pos + 0x05]);	   	    				 
                 pos += len + 0x0A;
                 break;
      case 0x15: 
				 len = Get3 (&mem[pos + 0x05]);
				 Put2(pause, &mem[pos + 0x02]);	   	    
				 block++;
	  			 direct = 1;
                 pos += Get3 (&mem[pos + 0x05]) + 0x08;
                 break;
      case 0x20: 
				 Put2(pause, &mem[pos]);	
				 block++;
				 pos += 0x02; 
				 break;
      case 0x21: pos += mem[pos + 0x00] + 0x01; break;
      case 0x22: break;
      case 0x23: pos += 0x02; break;
      case 0x30: pos += mem[pos + 0x00] + 0x01; break;
      case 0x31: pos += mem[pos + 0x01] + 0x02; break;
      case 0x32: pos += Get2 (&mem[pos + 0x00]) + 0x02; break;
      case 0x33: pos += (mem[pos + 0x00] * 0x03) + 0x01; break;
      case 0x34: pos += 0x08; break;
      case 0x35: pos += Get4 (&mem[pos + 0x10]) + 0x14; break;
      case 0x40: pos += Get3 (&mem[pos + 0x08]) + 0x0B; break;
      case 0x5A: pos += 0x09; break;
      default:   pos += Get4 (&mem[pos + 0x00] + 0x04);
                 not_rec = 1;
      }
    }
	
  if (fwrite (mem, 1, flen-10, fho) != (flen-10))
    Error ("Write error!");

  fclose (fhi);
  fclose (fho);
  free (mem);
  //system("pause");
  return (0);
}

/* Changes the File Basename of String *str with *suff */
void ChangeFileBasename (char *str, char *suff)
{
  int n;
  char *ext=".tzx";
  
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

