///////////////////////////////////////////////////////////////////////////////
// TAP to TZX converter
//                                                                       v0.12b
// (c) 1997 Tomaz Kac
// Modified for compilation with gcc: gcc -o tap2tzx tap2tzx.c

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int fhi, fho, flen;
unsigned char *mem;
char buf[256];
int pos;
int len;
int block;
char tzxbuf[10] = {'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1A, 1, 03};

void Error(char *errstr)
{
    printf("\n-- Error: %s\n", errstr);
    exit(0);
}

int FileLength(int fh)
{
    int curpos, size;
    curpos = lseek(fh, 0, SEEK_CUR);
    size = lseek(fh, 0, SEEK_END);
    lseek(fh, curpos, SEEK_SET);
    return (size);
}

void ChangeFileExtension(char *str, char *ext)
{
    // Changes the File Extension of String *str to *ext
    int n, m;
    n = strlen(str);
    while (str[n] != '.' && n>0)
        n--;
    n++;
    str[n] = 0;
    strcat(str, ext);
}

int main(int argc, char *argv[])
{
    printf("\nZXTape Utilities - TAP to TZX Converter v0.12b\n");

    if (argc < 2 || argc > 3)
    {
        printf("\nUsage: TAP2TZX INPUT.TAP [OUTPUT.TZX]\n");
        exit(0);
    }
    if (argc == 2)
    {
        strcpy(buf, argv[1]);
        ChangeFileExtension(buf, "tzx");
    }
    else
        strcpy(buf, argv[2]);

//    fhi = open(argv[1], O_RDONLY);
    fhi = open(argv[1], O_RDONLY+O_BINARY);	
    if (fhi == -1)
        Error("Input file not found!");

//    fho = open(buf, O_WRONLY | O_CREAT | O_TRUNC,
//               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    fho = open(buf, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                 S_IRUSR | S_IWUSR);
				 
    flen = FileLength(fhi);

    mem = (unsigned char *)malloc(flen);
    if (mem == NULL)
        Error("Not enough memory to load input file!");

    read(fhi, mem, flen);
    write(fho, tzxbuf, 10);

    pos = block = 0;
    len = 1;
    //buf[0] = 0x10; buf[1] = 0xE8; buf[2] = 0x03;
	
	buf[0] = 0x11; 
	buf[1] = 0x80; buf[2] = 0x05; //Lpilotpulse {1408}
	buf[3] = 0x8D; buf[4] = 0x01; //Lsynch1 {397}
	buf[5] = 0x3D; buf[6] = 0x01; //Lsynch2 {317}
	buf[7] = 0x45; buf[8] = 0x01; //Lzero {325}
	buf[9] = 0x89; buf[10]= 0x02; //Lone {649}
	buf[11]= 0xE3; buf[12]= 0x12; //Npilotpulses {4835} {8063 header (flag<128), 3223 data (flag>=128)}
	buf[13]= 0x08; 	 	 //bitslLastbyte {8} 
	buf[14]= 0x3E; buf[15]= 0x01; //pausems 318
	buf[16]= 0xFF; buf[17]= 0xFF; buf[18]= 0xFF; //Ldata
				
    while (pos < flen && len)
    {
        len = mem[pos + 0] + mem[pos + 1] * 256;
        pos += 2;
        if (len)
        {
            if (pos + len >= flen)
            {
                //buf[1] = buf[2] = 0;
				buf[14] = buf[15] = 0;
            }
            //buf[3] = len & 0xff; buf[4] = len >> 8;
			buf[16] = len & 0xff; buf[17] = len >> 8; ; buf[18] = 0;
            write(fho, buf, 19);
            write(fho, &mem[pos], len);
        }
        pos += len;
        block++;
    }
    printf("\nSuccesfully converted %d blocks!\n", block);
    close(fhi);
    close(fho);
    free(mem);
}

