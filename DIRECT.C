///////////////////////////////////////////////////////////////////////////////
// Convert to Direct Recording block
//                                                                       v0.11
// (c) 1997 Tomaz Kac and Martijn van der Heide
//
// Watcom C 10.0+ specific code... Change file commands for other compilers

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//char *raw;	  	  // Address of the loaded .VOC file (converted to RAW)
unsigned char *raw;	 	 	 // Address of the loaded .VOC file (converted to RAW)
int rawlen;			// Length of the .VOC file in the memory
//char *dir;	  	  // Address of the converted .TZX direct recording block
unsigned char *dir;	 	 	 // Address of the converted .TZX direct recording block
int dirlen;			// Length ...
int rate;			// Sample Rate
int silence=0;		// Are Silence blocks in the .VOC file ?
char tstr[14];
char tzxbuf[10]={ 'Z','X','T','a','p','e','!', 0x1A, 1, 00 };
int ofh;			// Output File Handle
char buf2[256];

int val;					// Value of current pulse
int cur=0;					// Current position in the sample
int last;					// Position of last Pulse in the sample
double pulse;				// Length of current pulse read
int bytepos;				// Byte position in converted data
int bitpos;					// Bit            -||-
int same=1;					// Use 1:1 Conversion (only 8 times smaller) ?
int samples;				// Number of Samples that current pulse takes
double cycle;				// How much Z80 cycles is one Sample (byte)
int tsample;				// T-States/Sample (Re-Sampling frequency)
int start=0;				// Starting raw byte  of the conversion
int endb=0;					// Ending             -||-

char finp[255];   // Input File  (First Command Line Option)
char fout[255];   // Output File (Second Command Line Option or First with .TZX)
char errstr[255]; // Error String
int n=0;
int files=0;	  // Number of Files on the command line

#define TRES 127  // TRESHOLD Value !!! ( 127 )

void Error(char *errstr)
{
// exits with an error message *errstr

printf("\n-- Error: %s\n",errstr);
exit(0);
}

void ChangeFileExtension(char *str,char *ext)
{
// Changes the File Extension of String *str to *ext
int n,m;

n=strlen(str); while (str[n]!='.') n--;
n++; str[n]=0; strcat(str,ext);
}

int ReadVOCType(int fh, int *len, unsigned char *f)
{
// Reads the TYPE block from the VOC file... skips everything but DATA blocks
// Returns 0 if everything is OK and 1 if there was an unexpected End-Of-File
int l;
//char b;
//char buff[256];
unsigned char b;
unsigned char buff[256];
int t;

do	{
	if (!read(fh,&b,1)) return(1);
	if (!read(fh,buff,3)) return(1);
	*len=((int) (buff[0]) + ((int) (buff[1])*256) + ((int) (buff[2])*256*256));
	//printf("%x %d %x %x %x\n",b,*len, buff[0], buff[1], buff[2]);
	switch(b)
		{
		case 0x00:  *len=0; break;
		case 0x01:	*len-=2;
					if (!read(fh,f,1)) return(1);
					if (!read(fh,buff,1)) return(1);	// Any compression type... I don't care ;)
					break;
		case 0x02:	b=0x01; break;
		case 0x03:	silence=1;
		case 0x09:	*len-=12;
					if (!read(fh,&t,4)) return(1);
					*f=256-(1000000/t);	// Block 9 has normally written freq...
					if (!read(fh,buff,8)) return(1);
					break;
		default:	if (*len) lseek(fh, *len, SEEK_CUR); break;
		}
	} while (b!=0x01 && b!=0x00 && b!=0x09);
return(0);
}

int LoadVOC(char *filename)
{
// Loads a .VOC file to the memory
// Input   : filename
// Outputs : raw    - address of the loaded file
//           rawlen - length of the loaded file
//           rate   - sample rate of the file
// Returns : 0 - when everything went OK
//			 1 - when input file is not found
//           2 - when there is not enough memory to load file
//           3 - when the file is not the right type or corrupt

int fh;
int flen;
int len;
int plen;
char buf[256];
// char brate;          // Sample Rate in Creative Voice format
unsigned char brate;          // Sample Rate in Creative Voice format
int p=0;
//char dummy;
unsigned char dummy;
int ee=0;

if ((fh=open(filename,O_RDONLY | O_BINARY))==-1) return(1);
//flen=FileLength(fh);
flen=_filelength(fh);
raw=(char *) malloc(flen);
dir=(char *) malloc(flen>>1);
if (raw==NULL || dir==NULL) { close(fh); return(2); }
if (!read(fh,buf,19)) { close(fh); return(3); } buf[19]=0;
if (strcmp(buf,"Creative Voice File")) { close(fh); return(3); }
read(fh,buf,7);
if (ReadVOCType(fh, &len, &brate)) { close(fh); return(3); }
rate=(int) (1000000/(256-(int) brate));
printf("\nImplicit Sampling Rate: %5i Hz\n",rate);
printf("Loading File (%d bytes) ...\n",flen);
//printf("len %d\n", len);
while (len && ee==0)
	{
	plen=read(fh,raw+p,len); p+=plen;
	ee=ReadVOCType(fh,&len,&dummy);
	}
//printf("raw %x %x\n", *(raw+0), *(raw+1));
if (ee) printf("-- Warning: Unexpected End-Of-File !\n");
if (silence) printf("-- Warning: Silence blocks in file (Pauses will be wrong) !\n");
printf("Actual RAW size of file in memory: %d bytes\n",p);
if (!endb) endb=p-1;	// if no /end was supplied then convert WHOLE .VOC shit
close(fh);
rawlen=p;
return(0);
}

int GetPulse()
{
// Finds next Pulse and returns its length and value

last=cur;
if (raw[cur]>TRES) while(raw[cur]> TRES && cur<rawlen) cur++;
else			   while(raw[cur]<=TRES && cur<rawlen) cur++;
return(cur-last);
}

void Convert()
{
ofh=open(fout, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
//	             S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	           S_IRUSR | S_IWUSR );
write(ofh,tzxbuf,10);
cycle=(double) rate/3500000.0;
printf("\nCreating 1.00 version .TZX file with one Direct Recording block... \n\n");
cur=start;
bytepos=bitpos=dir[0]=0;
if (!same)	// Use this method when /t switch is used ... so :
	{		// Downsample (or UpSample) to (tsample) T-States/Sample
	printf("New Sample Rate: %5i Hz\n\n",3500000/tsample);
	if (raw[cur]>TRES) val=0xff; else val=0;
	while (cur<endb)
		{
		pulse=(double) GetPulse()/cycle;		// Pulse length in Z80 T-States
		samples=(int) (0.5+(pulse/(double) tsample));	// -||- in Samples
		if (!samples) samples=1;	// This is needed !
		while(samples)
			{
			dir[bytepos]<<=1; dir[bytepos]+=val&1; bitpos++;
			if (bitpos==8) { bytepos++; dir[bytepos]=0; bitpos=0; }
			samples--;
			}
		val=~val;
		}
	}
else 	// Make 1:1 sample (only 8 times smaller) (this is DEFAULT)
	{
	tsample=(int) (0.5+(3500000.0/(double) rate));
	//printf("cur %d raw[cur] %x \n",cur, raw[cur]);
	while (cur<endb)
		{
		//printf("cur %d bytepos %d bitpos %d raw[cur] %02X dir[bytepos] %02X \n", cur, bytepos, bitpos, raw[cur], dir[bytepos]); system("pause");
		if (raw[cur]>TRES) val=1; else val=0;
		dir[bytepos]<<=1; dir[bytepos]+=val; bitpos++;
		if (bitpos==8) { bytepos++; dir[bytepos]=0; bitpos=0; }
		cur++;
		}
	}
if (bitpos) bytepos++; else bitpos=8;
if (bitpos<8) dir[bytepos-1]<<=(8-bitpos);	// Shift the last byte properly
// Here we save the block
printf("Length of the block (using %d T-States/Sample) : %d bytes.\n",tsample,bytepos);
buf2[0]=0x15;
buf2[1]=(char) (tsample&0xff); buf2[2]=(char) (tsample>>8);
buf2[3]=buf2[4]=0; buf2[5]=bitpos;
buf2[6]=(char) (bytepos&0xff); buf2[7]=(char) ((bytepos>>8)&0xff);
buf2[8]=(char) ((bytepos>>8)>>8);
write (ofh,&buf2,9);
//printf("dir %d %x %x %x %x %x\n",bytepos, dir[0], dir[1], dir[2], dir[3], dir[4]);
write (ofh,dir,bytepos);
close (ofh);
}

void invalidoption(char *s)
{
// Prints the Invalid Option error

sprintf(errstr,"Invalid Option %s !",s);
Error(errstr);
}

int getnumber(char *s)
{
// Returns the INT number contained in string *s
int i;

sscanf(s,"%d",&i); return(i);
}

void main(int argc, char *argv[])
{
printf("\nZXTape Utilities - Direct Recording v0.11b\n");
if (argc<2)
	{
	printf("\nUsage: DIRECT [switches] INPUT.VOC [OUTPUT.TZX]\n");
	printf("\n       Switches:  /start n   Starting raw byte of conversion\n");
	printf("                  /end   n   Ending raw byte of conversion\n");
	printf("                  /t         Sample rate in Z80 T-States/Sample\n");
	printf("                             i.e. 79 for 44.1KHz and 158 for 22.05KHz\n");
	exit(0);									  
	}
for (n=1; n<argc; n++)
	{
	if (argv[n][0]=='/')
		{
		switch (argv[n][1])
			{
			case 's':	if (strcmp(argv[n],"/start")) invalidoption(argv[n]);
					 	start=getnumber(argv[n+1]); n++; break;
			case 'e':	if (strcmp(argv[n],"/end")) invalidoption(argv[n]);
					 	endb=getnumber(argv[n+1]); n++; break;
			case 't':	if (strcmp(argv[n],"/t")) invalidoption(argv[n]);
					 	same=0; tsample=getnumber(argv[n+1]); n++;break;
			default :	invalidoption(argv[n]);
			}
		}
	else
		{
		files++;
		switch (files)
			{
			case 1: strcpy(finp,argv[n]); break;
			case 2: strcpy(fout,argv[n]); break;
			default:Error("Too Many files on command line!");
			}
		}
	}
if (files==0) Error("No Files specified !");
if (files==1)
	{
	strcpy(fout,finp);
	ChangeFileExtension(fout,"TZX");
	}

switch (LoadVOC(finp))
	{
	case 1: Error("Input file not found!"); break;
	case 2: Error("Not enough memory to load input file!"); break;
	case 3: free(raw); free(dir);
	        Error("Input file corrupt or in a wrong format!"); break;
	}
Convert();
free(raw); free(dir);
}

