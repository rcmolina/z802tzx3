/******************************************************************************
**
** zlistbas
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

#define PROG_VER "1.02"
#define MAJREV 1        /* Major revision of the format this program supports */
#define MINREV 13        /* Minor revision -||- */


FILE *fIn;
unsigned short ProgAddr, LineNum, LineLen;
unsigned char LineData[65535],LineText[65535];

int k;
//char mem7old;
long flen;
unsigned char *mem;
char buf[256];
long pos, j;
int len;
int block;
int longer, custom, only, dataonly, direct, not_rec;
unsigned int PilotPulses, ticksPerSample, pause;

enum fflist{unknown, TZX, TAP, SNA, Z80, BAS} fformat = unknown;
enum cclist{cc0, cc1, cc2, cc3, cc4} colorcode = cc0;

int Get2 (unsigned char *mem) {return (mem[0] + (mem[1] * 256));}
int Get3 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256));}
int Get4 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256) + (mem[3] * 256 * 256 * 256));}

long FileLength (FILE* fh);
void Error (char *errstr);
void ChangeFileExtension (char *str, char *ext);
int DeTokenize(unsigned char *In, int LineLen, unsigned char *Out);
void ConCat(unsigned char *Out,int *Pos,unsigned char *Text);
void TZXPROC();
void TAPPROC();
int SNAPROC();
int Z80PROC();
void BASPROC();

int main (int argc, char *argv[])
{

  if (argc < 2 || argc > 3)
    {
    printf ("\nUsage: %s [option] file[.tzx|.tap|.sna|.z80|.bas] \n", argv[0]);
	printf("\n");
	printf ("options:\n");
	printf("         cc1      colour code format: \\{nn}\\{n} \n");
	printf("         cc2      colour code format: \\#0nn\\#00n \n");
	printf("         cc3      colour code format: \\{An} A=i|p|f|b|v|o\n");
	printf("         cc4      colour code format: \";CHR$ nn;CHR$ n;\" \n");
    exit (0);
    }

  if (argc == 2) 
    {  
    //strcpy (buf, argv[1]); 
    //ChangeFileExtension (buf, "txt");
	k = 1;
    }
  else{
    strcpy (buf, argv[1]);
    if (!strcmp(buf, "cc1")) colorcode=cc1;
    else if (!strcmp(buf, "cc2")) colorcode=cc2;
    else if (!strcmp(buf, "cc3")) colorcode=cc3;
    else if (!strcmp(buf, "cc4")) colorcode=cc4;	
    else Error ("option is not valid!");	
    k = 2;	  
  }

  if ((fIn = fopen (argv[k], "rb")) == NULL) 
    Error ("Can't read file!");

  flen = FileLength (fIn);

  if ((mem = (unsigned char *) malloc (flen)) == NULL) 
    Error ("Not enough memory to load input file!");

  if (fread (mem, 1, 10, fIn) != 10)
    Error ("Read error!");

  const unsigned char TZXSTART[]="ZXTape!";
  const unsigned char TAPSTART[]={0x13,0x00};
  const unsigned char SNAEXT[]=".SNA";
  const unsigned char Z80EXT[]=".Z80";
  const unsigned char BASSTART[]="PLUS3DO";

  //mem7old=mem[7]; 
  mem[7]=0;

  if (!strcasecmp((char *)argv[k] +strlen(argv[k]) -4, SNAEXT)) fformat=SNA;
  else if (!strcasecmp((char *)argv[k] +strlen(argv[k]) -4, Z80EXT)) fformat=Z80;
  else if (!strcmp((char *)mem, TZXSTART)) fformat=TZX;
  else if (!strcmp((char *)mem, TAPSTART)) fformat=TAP;
  else if (!strcmp((char *)mem, BASSTART)) fformat=BAS;
  else {free(mem); Error ("Unknown ZX file format!");} 

  //mem[7]=mem7old;
  fseek(fIn,0,SEEK_SET);
  if (fread (mem, 1, flen, fIn) != flen)
    Error ("Read error!");	 
  
  pos = block = longer = custom = only = dataonly = direct = not_rec;

  switch (fformat)
  	{
	case TAP:
			 TAPPROC();
			 break;
	case TZX: 
			 TZXPROC();
			 break;
	case SNA: 
			 SNAPROC();
			 break;
	case Z80: 
			 Z80PROC();
			 break;	   	   	    
	case BAS: 
			 BASPROC();
			 break;    
	}
	
  //system("pause");
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
  fclose (fIn);
  free (mem);  
  exit (0);
}


/*****************************************************************************
  DeTokenize()

  'In' should point to a string containing tokenized ZX Spectrum text.
  'LineLen' should contain the length of the Input data in characters.
  The expanded Text will be placed in 'Out', which must be large enough
  to contain the full text.
*****************************************************************************/
int DeTokenize(unsigned char *In,int LineLen,unsigned char *Out)
{
    int i = 0, o = 0;

    char cc1f[]= "\\{nn}\\{n}";
	char cc2f[]= "\\#0nn\\#00n";
	char cc3f[]= "\\{An}";
	char cc4f[]= "\";CHR$ nn;CHR$ n;\"";
	char *cc;

    for(i=0;i<LineLen;i++)
    {
        switch (In[i])
        {
        case 14:
            i += 5;   /* Skip encoded number */
            break;
        case 16:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='6';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='1';cc[4]='6';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='i';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='1';cc[8]='6';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   	    		
            i++;      /* embedded INK */
            break;
        case 17:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='7';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='1';cc[4]='7';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='p';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='1';cc[8]='7';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   	    	
            i++;      /* embedded PAPER */
            break;
        case 18:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='8';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='1';cc[4]='8';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='f';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='1';cc[8]='8';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   
            i++;      /* embedded FLASH */
            break;
        case 19:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='9';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='1';cc[4]='9';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='b';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='1';cc[8]='9';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;	   	    
	        }	   
            i++;      /* embedded BRIGHT */
            break;
        case 20:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='2';cc[3]='0';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='2';cc[4]='0';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='v';if (In[i+1]) cc[3]='i'; else cc[3]='n' ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='2';cc[8]='0';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;			  
	        }	   
            i++;      /* embedded INVERSE */
            break;
        case 21:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='2'; cc[3]='1';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 2:  cc=cc2f; cc[3]='2'; cc[4]='1';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 3:  cc=cc3f; cc[2]='o'; cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[7]='2'; cc[8]='1';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break; 	  
	        }	   
            i++;      /* embedded OVER */
            break;
        case 22:
            i += 2;   /* Skip embedded AT */
            break;
        case 23:
            i += 2;   /* Skip embedded TAB */
            break;
        case 92:
            ConCat(Out,&o,"\\\\");
            break;
        case 127:
            ConCat(Out,&o,"\\*"); /* Copyright symbol - compatible with zmakebas */
            break;               
        case 128:
            ConCat(Out,&o,"\\  ");
            break;
        case 129:
            ConCat(Out,&o,"\\ '");
            break;
        case 130:
            ConCat(Out,&o,"\\' ");
            break;
        case 131:
            ConCat(Out,&o,"\\''");
            break;
        case 132:
            ConCat(Out,&o,"\\ .");
            break;
        case 133:
            ConCat(Out,&o,"\\ :");
            break;
        case 134:
            ConCat(Out,&o,"\\'.");
            break;
        case 135:
            ConCat(Out,&o,"\\':");
            break;
        case 136:
            ConCat(Out,&o,"\\. ");
            break;
        case 137:
            ConCat(Out,&o,"\\.'");
            break;
        case 138:
            ConCat(Out,&o,"\\: ");
            break;
        case 139:
            ConCat(Out,&o,"\\:'");
            break;
        case 140:
            ConCat(Out,&o,"\\..");
            break;
        case 141:
            ConCat(Out,&o,"\\.:");
            break;
        case 142:
            ConCat(Out,&o,"\\:.");
            break;
        case 143:
            ConCat(Out,&o,"\\::");
            break;
        case 144:
            ConCat(Out,&o,"\\a");
            break;
        case 145:
            ConCat(Out,&o,"\\b");
            break;
        case 146:
            ConCat(Out,&o,"\\c");
            break;
        case 147:
            ConCat(Out,&o,"\\d");
            break;
        case 148:
            ConCat(Out,&o,"\\e");
            break;
        case 149:
            ConCat(Out,&o,"\\f");
            break;
        case 150:
            ConCat(Out,&o,"\\g");
            break;
        case 151:
            ConCat(Out,&o,"\\h");
            break;
        case 152:
            ConCat(Out,&o,"\\i");
            break;
        case 153:
            ConCat(Out,&o,"\\j");
            break;
        case 154:
            ConCat(Out,&o,"\\k");
            break;
        case 155:
            ConCat(Out,&o,"\\l");
            break;
        case 156:
            ConCat(Out,&o,"\\m");
            break;
        case 157:
            ConCat(Out,&o,"\\n");
            break;
        case 158:
            ConCat(Out,&o,"\\o");
            break;
        case 159:
            ConCat(Out,&o,"\\p");
            break;
        case 160:
            ConCat(Out,&o,"\\q");
            break;
        case 161:
            ConCat(Out,&o,"\\r");
            break;
        case 162:
            ConCat(Out,&o,"\\s");
            break;
        case 163:
            ConCat(Out,&o,"\\t");
            break;
        case 164:
            ConCat(Out,&o,"\\u");
            break;
        case 165:
            ConCat(Out,&o,"RND");
            break;
        case 166:
            ConCat(Out,&o,"INKEY$");
            break;
        case 167:
            ConCat(Out,&o,"PI");
            break;
        case 168:
            ConCat(Out,&o,"FN ");
            break;
        case 169:
            ConCat(Out,&o,"POINT ");
            break;
        case 170:
            ConCat(Out,&o,"SCREEN$ ");
            break;
        case 171:
            ConCat(Out,&o,"ATTR ");
            break;
        case 172:
            ConCat(Out,&o,"AT ");
            break;
        case 173:
            ConCat(Out,&o,"TAB ");
            break;
        case 174:
            ConCat(Out,&o,"VAL$ ");
            break;
        case 175:
            ConCat(Out,&o,"CODE ");
            break;
        case 176:
            ConCat(Out,&o,"VAL ");
            break;
        case 177:
            ConCat(Out,&o,"LEN ");
            break;
        case 178:
            ConCat(Out,&o,"SIN ");
            break;
        case 179:
            ConCat(Out,&o,"COS ");
            break;
        case 180:
            ConCat(Out,&o,"TAN ");
            break;
        case 181:
            ConCat(Out,&o,"ASN ");
            break;
        case 182:
            ConCat(Out,&o,"ACS ");
            break;
        case 183:
            ConCat(Out,&o,"ATN ");
            break;
        case 184:
            ConCat(Out,&o,"LN ");
            break;
        case 185:
            ConCat(Out,&o,"EXP ");
            break;
        case 186:
            ConCat(Out,&o,"INT ");
            break;
        case 187:
            ConCat(Out,&o,"SQR ");
            break;
        case 188:
            ConCat(Out,&o,"SGN ");
            break;
        case 189:
            ConCat(Out,&o,"ABS ");
            break;
        case 190:
            ConCat(Out,&o,"PEEK ");
            break;
        case 191:
            ConCat(Out,&o,"IN ");
            break;
        case 192:
            ConCat(Out,&o,"USR ");
            break;
        case 193:
            ConCat(Out,&o,"STR$ ");
            break;
        case 194:
            ConCat(Out,&o,"CHR$ ");
            break;
        case 195:
            ConCat(Out,&o,"NOT ");
            break;
        case 196:
            ConCat(Out,&o,"BIN ");
            break;
        case 197:
            ConCat(Out,&o," OR ");
            break;
        case 198:
            ConCat(Out,&o," AND ");
            break;
        case 199:
            ConCat(Out,&o,"<=");
            break;
        case 200:
            ConCat(Out,&o,">=");
            break;
        case 201:
            ConCat(Out,&o,"<>");
            break;
        case 202:
            ConCat(Out,&o," LINE ");
            break;
        case 203:
            ConCat(Out,&o," THEN ");
            break;
        case 204:
            ConCat(Out,&o," TO ");
            break;
        case 205:
            ConCat(Out,&o," STEP ");
            break;
        case 206:
            ConCat(Out,&o," DEF FN ");
            break;
        case 207:
            ConCat(Out,&o," CAT ");
            break;
        case 208:
            ConCat(Out,&o," FORMAT ");
            break;
        case 209:
            ConCat(Out,&o," MOVE ");
            break;
        case 210:
            ConCat(Out,&o," ERASE ");
            break;
        case 211:
            ConCat(Out,&o," OPEN #");
            break;
        case 212:
            ConCat(Out,&o," CLOSE #");
            break;
        case 213:
            ConCat(Out,&o," MERGE ");
            break;
        case 214:
            ConCat(Out,&o," VERIFY ");
            break;
        case 215:
            ConCat(Out,&o," BEEP ");
            break;
        case 216:
            ConCat(Out,&o," CIRCLE ");
            break;
        case 217:
            ConCat(Out,&o," INK ");
            break;
        case 218:
            ConCat(Out,&o," PAPER ");
            break;
        case 219:
            ConCat(Out,&o," FLASH ");
            break;
        case 220:
            ConCat(Out,&o," BRIGHT ");
            break;
        case 221:
            ConCat(Out,&o," INVERSE ");
            break;
        case 222:
            ConCat(Out,&o," OVER ");
            break;
        case 223:
            ConCat(Out,&o," OUT ");
            break;
        case 224:
            ConCat(Out,&o," LPRINT ");
            break;
        case 225:
            ConCat(Out,&o," LLIST ");
            break;
        case 226:
            ConCat(Out,&o," STOP ");
            break;
        case 227:
            ConCat(Out,&o," READ ");
            break;
        case 228:
            ConCat(Out,&o," DATA ");
            break;
        case 229:
            ConCat(Out,&o," RESTORE ");
            break;
        case 230:
            ConCat(Out,&o," NEW ");
            break;
        case 231:
            ConCat(Out,&o," BORDER ");
            break;
        case 232:
            ConCat(Out,&o," CONTINUE ");
            break;
        case 233:
            ConCat(Out,&o," DIM ");
            break;
        case 234:
            ConCat(Out,&o," REM ");
            break;
        case 235:
            ConCat(Out,&o," FOR ");
            break;
        case 236:
            ConCat(Out,&o," GO TO ");
            break;
        case 237:
            ConCat(Out,&o," GO SUB ");
            break;
        case 238:
            ConCat(Out,&o," INPUT ");
            break;
        case 239:
            ConCat(Out,&o," LOAD ");
            break;
        case 240:
            ConCat(Out,&o," LIST ");
            break;
        case 241:
            ConCat(Out,&o," LET ");
            break;
        case 242:
            ConCat(Out,&o," PAUSE ");
            break;
        case 243:
            ConCat(Out,&o," NEXT ");
            break;
        case 244:
            ConCat(Out,&o," POKE ");
            break;
        case 245:
            ConCat(Out,&o," PRINT ");
            break;
        case 246:
            ConCat(Out,&o," PLOT ");
            break;
        case 247:
            ConCat(Out,&o," RUN ");
            break;
        case 248:
            ConCat(Out,&o," SAVE ");
            break;
        case 249:
            ConCat(Out,&o," RANDOMIZE ");
            break;
        case 250:
            ConCat(Out,&o," IF ");
            break;
        case 251:
            ConCat(Out,&o," CLS ");
            break;
        case 252:
            ConCat(Out,&o," DRAW ");
            break;
        case 253:
            ConCat(Out,&o," CLEAR ");
            break;
        case 254:
            ConCat(Out,&o," RETURN ");
            break;
        case 255:
            ConCat(Out,&o," COPY ");
            break;
        default:
            if (In[i] >= 32) Out[o++] = In[i];
        }
    }

    Out[o]=0;
    return strlen(Out);
}

/*****************************************************************************
  ConCat()

  Places the null-terminated string pointed to by 'Text' at position 'Pos'
  in 'Out'. 'Out' must be large enough to contain the newly placed text.
*****************************************************************************/
void ConCat(unsigned char *Out,int *Pos,unsigned char *Text)
{
    int n = 0;
    
    /* Stop two adjacent spaces appearing */
    if ( (*Pos > 0) && (Out[*Pos-1] == 32) && (Text[0] == 32) ) (*Pos)--; 

    while(Text[n] != 0)
    {
        Out[*Pos+n] = Text[n];
        n++;
    }

    *Pos += n;
}

void TZXPROC()
{
    pos = 10;
    j = pos;
    while (pos < flen){	
	     switch (mem[10])
	        {
	        case 0x10:
				len = Get2(&mem[pos +1 +2 +2 +0x13 +1 +2]);	   //Data Block length
			    while ((5 +0x13 +5 +1 + j <  pos +1 +2 +2 +0x13 +1 +2 +2 +len -1) && (mem[pos +5 +1]==0)) {
			
					LineNum = 256*mem[5 +0x13 +5 +1 +j] + mem[5 +0x13 +5 +2 +j];
			        if (LineNum > 16384) break; //se salta la zona de vars tras programa
			
			        LineLen = mem[5 +0x13 +5 +3 +j] + 256*mem[5 +0x13 +5 +4 +j];
                    if (LineLen > len) LineLen= len;
			
					memcpy(LineData,mem+5 +0x13 +5 +5 +j ,LineLen);
			        LineData[LineLen]=0; // Terminate the line data
					
			        DeTokenize(LineData,LineLen,LineText);
					printf("%d%s\n",LineNum,LineText);
					
					j= j + 2 +2 + LineLen;      
			    }
				if (mem[pos +5 +1]==0) printf("\n");
				pos = pos +1 +2 +2 +0x13 + 1 +2 +2 +len; // header block length= id(1) +pause(2) +tzx block length(2) +0x13
				j= pos;	 								 // data block length=  id(1) +pause(2)  + tzx block length(2) +len	   
	
			    break;
		    case 0x11:
				len = Get3(&mem[pos +1 +13 +2 +3 +0x13 +1 +13 +2]);	   //Data Block length
			    while ((19 +0x13 +19 +1 + j <  pos +1 +13 +2 +3 +0x13 +1 +13 +2 +3 +len -1) && (mem[pos + 19 +1]==0)) {
			
					LineNum = 256*mem[19 +0x13 +19 +1 +j] + mem[19 +0x13 +19 +2 +j];
			        if (LineNum > 16384) break; //se salta la zona de vars tras programa
			
			        LineLen = mem[19 +0x13 +19 +3 +j] + 256*mem[19 +0x13 +19 +4 +j];
					if (LineLen > len) LineLen= len;
			
					memcpy(LineData,mem+19 +0x13 +19 +5 +j ,LineLen);
			        LineData[LineLen]=0; // Terminate the line data
					
			        DeTokenize(LineData,LineLen,LineText);
					printf("%d%s\n",LineNum,LineText);
					
					j= j + 2 +2 + LineLen;      
			    }
				if (mem[pos + 19 +1]==0) printf("\n");
				pos = pos +1 +13 +2 +3 +0x13 + 1 +13 +2 +3 +len; // header block length= id(1) +extra(13) +pause(2) +tzx length(3) +0x13
				j= pos;	 	 	 	 	 	 	 	             // data block length=  id(1) +extra (13) +pause(2)  + tzx length(3) + len

			    break;
		    default:
		        break;
		    }
    }	
}

void TAPPROC()
{
	pos = 0;
	j = pos;
    while (pos < flen){
          len = Get2(&mem[pos + 0x15]);	        //Data Block length
		  while ((0x18 + j < pos +2 +0x13 +2 +len -1) && (mem[pos + 0x03]==0)) {

				LineNum = 256*mem[0x18 + j] + mem[0x19 + j];
				if (LineNum > 16384) break;

				LineLen = mem[0x1A + j] + 256*mem[0x1B + j];
                if (LineLen > len) LineLen= len;
				
				memcpy(LineData,mem+0x1C+j ,LineLen);
		        LineData[LineLen]=0; // Terminate the line data
		
		        DeTokenize(LineData,LineLen,LineText);
		
                printf("%d%s\n",LineNum,LineText);
				j=j+2+2+LineLen;
				
          }
		  if (mem[pos + 0x03]==0) printf("\n");
		  pos = pos +2 +0x13 +2 +len; // header block length= 2 + 0x13 and data block length= 2 + len
		  j= pos;
    }

}

int SNAPROC()
{
    /* All 48K snapshots are 49179 bytes, check to ensure our .sna is at least this long */
    if (flen < 49179) {
        fprintf(stderr,".SNA files should be at least 49179 bytes long, the input file is %d bytes\n",flen);
        return(3);
    }

    /* Get the PROG system variable (PEEK 23635+256*PEEK 23636) */
	pos=7278;
	LineData[0]= mem[pos]; LineData[1]= mem[pos+1];
    ProgAddr = LineData[0] | LineData[1]<<8;

    if ((ProgAddr < 23296) || (ProgAddr > 65530)){
        fprintf(stderr,"Invalid system variable area. This snapshot does not contain a BASIC program.\n");
        return(4);
    }

    /* seek to PROG area */
	pos=ProgAddr-16384+27;
    while (pos < flen) {	
        LineNum = 256*mem[pos] + mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = mem[pos +2] + 256*mem[pos +3];
		if (LineLen > flen) LineLen= flen -(pos+4);

		memcpy(LineData,mem+pos+4 ,LineLen);
        LineData[LineLen]=0; //Terminate the line data

        DeTokenize(LineData,LineLen,LineText);
        printf("%d%s\n",LineNum,LineText);
		
		pos= pos +2 +2 + LineLen; 	        
    }
	printf("\n");
	return(0);

}

int Z80PROC()
{

#define printblock(buff,len,adr0) \
    for (i=0;i<len;i+=16) { \
        printf ("#%04x:  ",i+adr0); for (j=0;j<16;j++) { \
            printf ("%02x ",buff[i+j]); \
            if (j==7) printf (" "); if (j==15) printf ("\n"); } } \
    printf ("\n");

#define word unsigned short int
#define byte unsigned char

word i;
byte ver,j,rommod,verbose=0;
byte *buf1,*buf2, *buf3;

unsigned short int unpack(unsigned char *inp, unsigned char *outp, unsigned short int size)
{
    register unsigned short int incount=0,outcount=0;
    unsigned short int i;
    unsigned char j;
    do {
       if ((inp[0]==0xED)&&(inp[1]==0xED)) {
            i=inp[2];
            j=inp[3];
            inp+=4;
            incount+=4;
            outcount+=i;
            for (;i!=0;i--) *(outp++)=j;
       } else {
         *(outp++)=*(inp++);
         incount++;
         outcount++;
       }
    } while (outcount<size);
    if (outcount!=size) incount=0;
    return (incount);
}
     pos =0;
     //fread(&buffer,1,30,fin);
	 ver=0;
	 if (mem[6]==0) {		// pc = for z80 ver >=2
		 //fread(&(buffer.length),1,2,fin); // z80 version
		 len = Get2(&mem[30]);
		 switch (len) {
	        case 21:
	        case 23:
			     //printf ("(Version 2.0 .Z80 file)\n\n");
	             ver=2;
	             break;
	        case 54:
	             //printf ("(Version 3.0 .Z80 file)\n\n");
	             ver=3;
	             break;
	        default:
		         printf ("\nUnsupported version of .Z80 file, or .Z80 file corrupt\n\n");
		         return(1);
		    }
			//fread(&(buffer.rpc2),1,buffer.length,fin);
			pos= 32 +len;
     } else {
	     //printf ("(Version 1.45 or earlier .Z80 file)\n\n");
		 pos = 30;  
     }
	
    if (ver<2) {
         buf1=(unsigned char*)malloc(49152L+256);
         buf2=(unsigned char*)malloc(49152L+256);
		 
		 //pos=30; fseek(fIn, pos,SEEK_SET);fread(buf1,1,49152L+256,fIn);
		 pos=30; memcpy(buf1, mem +pos ,flen -pos);
		    
		 //if (buffer.rr_bit7&32) unpack(buf1,buf2,49152L);
		 if (mem[12]&32) {unpack(buf1,buf2,49152L); buf3=buf2; flen =49152; }         //Block of data is compressed, bit5 ON
		 else buf3=buf1;

		 //printblock(buf2,49152L,16384)	 	 // buf2
		 pos = flen;
		 
	} else {
	     buf1=(unsigned char*)malloc(16384+256);
	     buf2=(unsigned char*)malloc(16384+256);
         buf3=(unsigned char*)malloc(49152L+256);
		 	    
	     //i=fread(&blockhdr,1,3,fin);			// 3 bytes: 2 bytes for length and 1 byte for pagenumber
		 
	     while (pos < flen) {
		    len = Get2(&mem[pos]);
            //printf ("Page nr: %2d      Block length: %d\n", mem[pos -1], len);
            //i=fread(buf1,1,blockhdr.length,fin);
		    memcpy(buf1, mem +pos +3, len);
					
	 	    unpack(buf1,buf2,16384);
			
			switch (mem[pos +2])
			  {
			  case 4: memcpy(buf3 + 16384, buf2, 16384); break; // page 4: 8000-bfff
			  case 5: memcpy(buf3 + 32768, buf2, 16384); break; // page 5: c000-ffff
			  case 8: memcpy(buf3, buf2, 16384); break;         // page 8: 4000-7fff
			  }
			
	 	    //printblock(buf2,16384,0)
			
		    //i=fread(&blockhdr,1,3,fin);
			pos= pos + 3 + len;
	     }
		 flen =49152;
		 //printblock(buf3,49152,0)
    }


    // Get the PROG system variable (PEEK 23635+256*PEEK 23636)
	pos=23635 - 16384;
	LineData[0]= buf3[pos]; LineData[1]= buf3[pos+1];
    ProgAddr = LineData[0] | LineData[1]<<8;

    if ((ProgAddr < 23296) || (ProgAddr > 65530)){
        fprintf(stderr,"Invalid system variable area. This snapshot does not contain a BASIC program.\n");
        return(4);
    }

    // seek to PROG area
	pos=ProgAddr-16384;
    while (pos < flen) {	
        LineNum = 256*buf3[pos] + buf3[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = buf3[pos +2] + 256*buf3[pos +3];
        if (LineLen > flen) LineLen= flen -(pos+4);

		memcpy(LineData, buf3 +pos +4 ,LineLen);
        LineData[LineLen]=0; //Terminate the line data

        DeTokenize(LineData,LineLen,LineText);
        printf("%d%s\n",LineNum,LineText);
		
		pos= pos +2 +2 + LineLen; 	        
    }
	printf("\n");
	return(0);	      	   


}

void BASPROC()
{
	len = Get4(&mem[0x0B]);
	pos = 128;
    while ((pos < len) && (mem[0x0F]==0)) {

		LineNum = 256*mem[pos] + mem[pos +1];
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

        LineLen = mem[pos +2] + 256*mem[pos +3];
        if (LineLen > len) LineLen= len;

		memcpy(LineData,mem+pos+4 ,LineLen);
        LineData[LineLen]=0; // Terminate the line data
		
        DeTokenize(LineData,LineLen,LineText);
		printf("%d%s\n",LineNum,LineText);
		
		pos= pos +2 +2 + LineLen;      
    }
	if (mem[0x0F]==0) printf("\n");
}

