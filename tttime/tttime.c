/******************************************************************************
**
** Total Tape Time
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
#define MINREV 3        /* Minor revision -||- */

FILE *fhi;
long flen;
unsigned char *mem;
char buf[256];
long pos;
int len;
int block;
int longer, custom, only, dataonly, direct, not_rec;
unsigned char tzxbuf[10] = {'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1A, 1, 0};
unsigned long ustape, BLKnumOnes; 
unsigned int ticksOnePulse, ticksZeroPulse, ticksPilotPulse, ticksSync1Pulse, ticksSync2Pulse, PilotPulses, ticksPerSample, pause;
unsigned int mins, segs;
enum Tapelist{unknown, TAP, TZX} tape = unknown;

int Get2 (unsigned char *mem) {return (mem[0] + (mem[1] * 256));}
int Get3 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256));}
int Get4 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256) + (mem[3] * 256 * 256 * 256));}

long FileLength (FILE* fh);
void Error (char *errstr);
void ChangeFileExtension (char *str, char *ext);
unsigned long BlockCountedOnes (unsigned long posini, unsigned long posend);
void TZXPROC();
void TAPPROC();

int main (int argc, char *argv[])
{
  //printf ("\n - Tape Total Time duration\n");

  if (argc < 2 || argc > 3)
    {
    printf ("\nUsage: tttime input.tzx \n");
    exit (0);
    }

  if (argc == 2) 
    {  
    strcpy (buf, argv[1]); 
    ChangeFileExtension (buf, "tap");
    }
  else      
    strcpy (buf, argv[2]);

  if ((fhi = fopen (argv[1], "rb")) == NULL) 
    Error ("Can't read file!");

  flen = FileLength (fhi);

  if ((mem = (unsigned char *) malloc (flen)) == NULL) 
    Error ("Not enough memory to load input file!");

  if (fread (mem, 1, 10, fhi) != 10)
    Error ("Read error!");

  mem[7] = 0;
  const unsigned char TAPSTART[]={0x13,0x00};

  if (strcmp ((char *)mem, "ZXTape!")) 
    { 
	 if ( strcmp ((char *)mem, TAPSTART) ) 
	   {
	    free (mem);
	    Error ("File is not in ZXTape format!");
	   }
	 else {
	  tape=TAP;
	  fseek(fhi,0,SEEK_SET);
	  if (fread (mem, 1, flen, fhi) != flen)
	    Error ("Read error!");	 
	 }  	
    }
  else {
	  tape=TZX;
	
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
  pos = block = longer = custom = only = dataonly = direct = not_rec = ustape = 0;
  
  switch (tape)
  	{
	case TAP:
			 TAPPROC();
			 break;
	
	case TZX: 
			 TZXPROC();
			 break;
	}
	

  printf ("\n");

  if (custom) 
    printf ("-- Warning: Custom Loading blocks calculated!\n\n");

  if (longer) 
    printf ("-- Warning: Over 64k long Custom Loading blocks were *not* calculated!\n\n");

  if (only) 
    printf ("-- Warning: Some Pure Tone and/or Sequence of Pulses blocks encountered!\n\n");

  if (dataonly) 
    printf ("-- Warning: Data Only blocks calculated!\n\n");

  if (direct) 
    printf ("-- Warning: Direct Recording blocks were encountered!\n\n");

  if (not_rec) 
    printf ("-- Warning: Some blocks were NOT recognised !\n\n");

  
  segs = (ustape+500000)/1000000;
  mins = segs/60;
  segs -= mins*60;
  
  printf ("Succesfully calculated %d blocks!, TAPE TIME: %ds (%02d:%d)\n", block, (ustape+500000)/1000000,mins,segs);
  fclose (fhi);
  free (mem);
  return (0);
}

/* Changes the File Extension of String *str to *ext */
void ChangeFileExtension (char *str, char *ext)
{
  int n;
  
  n = strlen(str); 

  while (str[n] != '.') 
    n--;

  n++; 
  str[n] = 0;
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


char numOnes(char x){
    char i = 0;
    if(x == 0)
        return 0;
    for(i = 1; x &= x-1; i++);
    return i;
}

unsigned long BlockCountedOnes (unsigned long posini, unsigned long posend) {
	unsigned long popcnt;
	unsigned long inipos, endpos;
	int i;
	popcnt = 0;
	inipos = posini;
	endpos = posend;
	for(i = inipos; i <= endpos ; i++) {
		popcnt+= numOnes(mem[i]);
		//printf("\n pos %d byte: %d numOnes %d popcnt %d\n", i, mem[i],numOnes(mem[i]),popcnt);
	}
    return popcnt;	    
}

void TZXPROC()
{
  while (pos < flen - 10)
    {
    pos++;
    switch (mem[pos - 1])
      {
      case 0x10: 	 	 	  
				 ticksZeroPulse = 855;				 
				 ticksOnePulse = 1710;
				 
				 if (mem[pos + 0x04] < 128)
				 	ustape += (8063*2168 + 667 +735)/3.5;
				 else 
				 	ustape += (3223*2168 + 667 +735)/3.5;
						  	  	  	   
	  			 len = Get2(&mem[pos + 0x02]);
				 
				 BLKnumOnes = BlockCountedOnes(pos + 0x04, pos + 0x04 + len -1);

				 ustape += ( (len *8 - BLKnumOnes)* 2*ticksZeroPulse + (BLKnumOnes* 2*ticksOnePulse) )/3.5;
				 
	  			 pause = Get2(&mem[pos]); //pausa en ms
				 ustape += 1000*pause;
					
				 //printf("\n Len: %dchars, Ones: %dbits, acctime: %dus\n", len, BLKnumOnes, ustape);
				 printf("Block %3d Length %6d Normal Speed ,Pause: %5d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);
				 
                 pos += len + 0x04;
                 block++;
                 break;
 
      case 0x11: ticksPilotPulse = Get2(&mem[pos]);
	  			 ticksSync1Pulse = Get2(&mem[pos + 0x02]);
				 ticksSync2Pulse = Get2(&mem[pos + 0x04]);
				 ticksZeroPulse = Get2(&mem[pos + 0x06]);
				 ticksOnePulse = Get2(&mem[pos + 0x08]);
				 PilotPulses = Get2(&mem[pos + 0x0A]);

				 ustape += (PilotPulses*ticksPilotPulse + ticksSync1Pulse + ticksSync2Pulse)/3.5;
				 
				 len = Get3 (&mem[pos + 0x0F]);
				 BLKnumOnes = BlockCountedOnes(pos + 0x12, pos + 0x12 + len -1);
				 ustape += ( (len *8 - BLKnumOnes)* 2*ticksZeroPulse + (BLKnumOnes* 2*ticksOnePulse) )/3.5;

				 pause = Get2(&mem[pos + 0x0D]); //pausa en ms
				 ustape += 1000*pause;
				 					
				 //printf("\n Len: %dchars, Ones: %dbits, acctime: %dus\n", len, BLKnumOnes, ustape);
				 printf("Block %3d Length %6d  Turbo Speed ,Pause: %5d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);	   	   	   	    
				 				 	 	 	 	  	  	  	  	   
                 if (len < 65536) 
				   block++;
                 else 
                   longer = 1;
				   
                 custom = 1;
                 pos += len + 0x12;
                 break;
				 
      case 0x12: ticksPilotPulse = Get2(&mem[pos]);
	  			 PilotPulses = Get2(&mem[pos + 0x02]);
				 ustape += (PilotPulses*ticksPilotPulse)/3.5;
	  			 only = 1;
                 pos += 0x04;
                 break;
      case 0x13: PilotPulses = Get2(&mem[pos]);
	  		     unsigned char i;
	  			 for (i=1; i<= PilotPulses ; i++) ustape += Get2(&mem[pos + 0x01 + 2*(i-1)]);
	  			 only = 1;
                 pos += (mem[pos + 0x00] * 0x02) + 0x01;
                 break;
      case 0x14: 
				 ticksZeroPulse = Get2(&mem[pos]);
				 ticksOnePulse = Get2(&mem[pos + 0x02]);
				 len = Get3 (&mem[pos + 0x07]);
				 
				 BLKnumOnes = BlockCountedOnes(pos + 0x0A, pos + 0x0A + len -1);
				 ustape += ( (len *8 - BLKnumOnes)* 2*ticksZeroPulse + (BLKnumOnes* 2*ticksOnePulse) )/3.5;
				 	 	 	 	  
				 pause = Get2(&mem[pos + 0x05]); //pausa en ms
				 ustape += 1000*pause;	     	 	  	     

				 printf("Block %3d Length %6d    Pure Data ,Pause: %5d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);
				 
                 if (len < 65536)
                   block++;
                 else 
                   longer = 1;
				   
                 dataonly = 1;
                 pos += len + 0x0A;
                 break;
      case 0x15: 
	  			 ticksPerSample = Get2(&mem[pos]);
				 len = Get3 (&mem[pos + 0x05]);
				 
				 //BLKnumOnes = BlockCountedOnes(pos + 0x08, pos + 0x08 + len -1);
				 //ustape += ( (len *8 - BLKnumOnes)* ticksPerSample + (BLKnumOnes* ticksPerSample) )/3.5;
				 ustape +=  len*8 * ticksPerSample /3.5;
				 
				 pause = Get2(&mem[pos + 0x02]); //pausa en ms
				 ustape += 1000*pause;	    		 	    

				 printf("Block %3d Length %6d   Direct Rec ,Pause: %5d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);
				 
				 block++;
	  			 direct = 1;
                 pos += Get3 (&mem[pos + 0x05]) + 0x08;
                 break;
      case 0x20: 
	  			 pause = Get2(&mem[pos]);
				 ustape += 1000*pause;
				 printf("Block %3d Length %6d  Pause Block ,Pause: %5d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);
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
}

void TAPPROC()
{

    while (pos < flen)
    {
		 ticksZeroPulse = 855;				 
		 ticksOnePulse = 1710;
		 
		 if (mem[pos + 0x02] < 128)
		 	ustape += (8063*2168 + 667 +735)/3.5;
		 else 
		 	ustape += (3223*2168 + 667 +735)/3.5;
				  	  	  	   
  		 len = Get2(&mem[pos]);	    
		 BLKnumOnes = BlockCountedOnes(pos + 0x02, pos + 0x02 + len -1);

		 ustape += ( (len *8 - BLKnumOnes)* 2*ticksZeroPulse + (BLKnumOnes* 2*ticksOnePulse) )/3.5;
		 
		 pause = 1000; //pausa en ms
		 ustape += 1000*pause;	     	 	  	
		 	 	 
		 printf("Block %3d Length %6d Normal Speed ,GAP: %3d ms ,ETA: %6d ms \n", block, len , pause, ustape/1000);
		 
         pos += len + 0x02;
         block++;
				 
    }
}

