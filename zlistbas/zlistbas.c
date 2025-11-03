/******************************************************************************
**
** zlistbas  rcmolina@gmail.com
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

#include <stdint.h>
/* 7.18.1.1  Exact-width integer types */
/*
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;
*/

#define PROG_VER "1.16"

int inFirstLineREM; /* 1=First line is a REM and we are on the first line */
int onlyFirstLineREM = 0; /* 1=Only preserve codes in a first line REM, 0=Preserve codes everywhere */
#define QUOTE_code 11
#define NUM_code 126
#define REM_code 234
#define REM_zx80code 254

#define NAK "#" // Not A Kharacter

char *charset_zx80zmb[] =
{
/* 000-009 */ " ","\"","\\: ","\\..","\\' ","\\ '","\\. ","\\ .","\\.'","\\!:",
/* 010-019 */ "\\!.","\\!'","\\\\","$",":","?","(",")",">","<",
/* 020-029 */ "=","+","-","*","/",";",",",".","0","1",
/* 030-039 */ "2","3","4","5","6","7","8","9","A","B",
/* 040-049 */ "C","D","E","F","G","H","I","J","K","L",
/* 050-059 */ "M","N","O","P","Q","R","S","T","U","V",
/* 060-069 */ "W","X","Y","Z",NAK,NAK,NAK,NAK,NAK,NAK,
/* 070-079 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 080-089 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 090-099 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 100-109 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 110-119 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 120-129 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,"\\::","\\\"",
/* 130-139 */ "\\ :","\\''","\\.:","\\:.","\\':","\\:'","\\'.","\\|:","\\|.","\\|'",
/* 140-149 */ "\\@","\\$","\\:","\\?","\\(","\\)","\\>","\\<","\\=","\\+",
/* 150-159 */ "\\-","\\*","\\/","\\;","\\,","\\.","\\0","\\1","\\2","\\3",
/* 160-169 */ "\\4","\\5","\\6","\\7","\\8","\\9","a","b","c","d",
/* 170-179 */ "e","f","g","h","i","j","k","l","m","n",
/* 180-189 */ "o","p","q","r","s","t","u","v","w","x",
/* 190-199 */ "y","z",NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 200-209 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 210-219 */ NAK,NAK,"\""," THEN"," TO ",";",",",")","(","NOT",
/* 220-229 */ "-","+","*","/"," AND "," OR ","**","=",">","<",
/* 230-239 */ " LIST"," RETURN"," CLS "," DIM "," SAVE "," FOR "," GOTO ",
		" POKE "," INPUT "," RANDOMIZE ",
/* 240-249 */ " LET ",NAK,NAK," NEXT "," PRINT ",
		NAK," NEW"," RUN "," STOP "," CONTINUE ",
/* 250-255 */ " IF "," GOSUB "," LOAD"," CLEAR"," REM ",NAK
};

char *charset_zx81zmb[] =
{
/* 000-009 */ " ","\\' ","\\ '","\\''","\\. ","\\: ","\\.'","\\:'","\\!:","\\!.",
/* 010-019 */ "\\!'","\"","\\\\","$",":","?","(",")",">","<",
/* 020-029 */ "=","+","-","*","/",";",",",".","0","1",
/* 030-039 */ "2","3","4","5","6","7","8","9","A","B",
/* 040-049 */ "C","D","E","F","G","H","I","J","K","L",
/* 050-059 */ "M","N","O","P","Q","R","S","T","U","V",
/* 060-069 */ "W","X","Y","Z","RND","INKEY$ ","PI",NAK,NAK,NAK,
/* 070-079 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 080-089 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 090-099 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 100-109 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 110-119 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 120-129 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,"\\::","\\.:",
/* 130-139 */ "\\:.","\\..","\\':","\\ :","\\'.", "\\ .","\\|:","\\|.","\\|'","\\\"",
/* 140-149 */ "\\@","\\$","\\:","\\?","\\(","\\)","\\>","\\<","\\=","\\+",
/* 150-159 */ "\\-","\\*","\\/","\\;","\\,","\\.","\\0","\\1","\\2","\\3",
/* 160-169 */ "\\4","\\5","\\6","\\7","\\8","\\9","a","b","c","d",
/* 170-179 */ "e","f","g","h","i","j","k","l","m","n",
/* 180-189 */ "o","p","q","r","s","t","u","v","w","x",
/* 190-199 */ "y","z","`","AT ","TAB ",NAK,"CODE ","VAL ","LEN ","SIN ",
/* 200-209 */ "COS ","TAN ","ASN ","ACS ","ATN ","LN ","EXP ",
		"INT ","SQR ","SGN ",
/* 210-219 */ "ABS ","PEEK ","USR ","STR$ ","CHR$ ","NOT ",
		"**"," OR "," AND ","<=",
/* 220-229 */ ">=","<>"," THEN"," TO "," STEP "," LPRINT ",
		" LLIST "," STOP"," SLOW"," FAST",
/* 230-239 */ " NEW"," SCROLL"," CONT "," DIM "," REM "," FOR "," GOTO ",
		" GOSUB "," INPUT "," LOAD ",
/* 240-249 */ " LIST "," LET "," PAUSE "," NEXT "," POKE ",
		" PRINT "," PLOT "," RUN "," SAVE "," RAND ",
/* 250-255 */ " IF "," CLS"," UNPLOT "," CLEAR"," RETURN"," COPY"
};

char **charset = charset_zx81zmb;

FILE *fIn;
unsigned short ProgAddr, VarsAddr, LineNum, LineLen;
unsigned char LineData[65535],LineText[65535];

int k;
long flen;
unsigned char *mem, *zmem;
char buf[256];
int rzxcnt=0;
long pos, j;
int len, ProgLen;
int block;
int longer, custom, only, dataonly, direct, not_rec;
unsigned int PilotPulses, ticksPerSample, pause;

enum fflist{unknown, TZX, RZX, TAP, SP, SNA, Z80, BAS, O, P, P81, T81} fformat = unknown;
enum cclist{cc0, cc1, cc2, cc3, cc4, cc5} colorcode = cc1;

int Get2 (unsigned char *mem) {return (mem[0] + (mem[1] * 256));}
int Get3 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256));}
int Get4 (unsigned char *mem) {return (mem[0] + (mem[1] * 256) + (mem[2] * 256 * 256) + (mem[3] * 256 * 256 * 256));}
int zlen(unsigned char *mem) {int i= 0;if (mem[i]==0) return(0);while (mem[i]<128) i++; return(i+1);}
int zlinelen(unsigned char *mem) {int i= 0; while (mem[i] != 0x76) i++; return(i+1); }
int GetT81L (unsigned char *mem)
{
int i= 0, j= 0, len= 0;
int pow10(int j) { int k= 1; while (j > 0) {k= k*10 ; j--;} return (k);}
while (mem[i]) i++;
while (i>0) {
  len=len +(mem[i-1] -48) * pow10(j);
  i--;
  j++;
}
return len;
}

long FileLength (FILE* fh);
void Error (char *errstr);
void ChangeFileExtension (char *str, char *ext);
int DeTokenize(unsigned char *In, int LineLen, unsigned char *Out);
void ConCat(unsigned char *Out,int *Pos,unsigned char *Text);
int TZXPROC();
int RZXPROC();
void TAPPROC();
int SPPROC();
int SNAPROC();
int Z80PROC();
void BASPROC();
int OPROC();
int PPROC();
int P81PROC();

/*
 * zlib-deflate-nostdlib
 *
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#define DEFLATE_ERR_INPUT_LENGTH (-1)
#define DEFLATE_ERR_METHOD (-2)
#define DEFLATE_ERR_FDICT (-3)
#define DEFLATE_ERR_BLOCK (-4)
#define DEFLATE_ERR_CHECKSUM (-5)
#define DEFLATE_ERR_OUTPUT_LENGTH (-6)
#define DEFLATE_ERR_FCHECK (-7)
#define DEFLATE_ERR_NLEN (-8)
#define DEFLATE_ERR_HUFFMAN (-9)

int32_t inflate(const unsigned char *input_buf, uint32_t input_len, unsigned char *output_buf, uint32_t output_len);
int32_t inflate_zlib(const unsigned char *input_buf, uint32_t input_len, unsigned char *output_buf, uint32_t output_len);

//
// The compressed (inflated) input data.
//
const unsigned char *deflate_input_now;
const unsigned char *deflate_input_end;

//
// The decompressed (deflated) output stream.
//
unsigned char *deflate_output_now;
unsigned char *deflate_output_end;

//
// The current bit offset in the input stream, if any.
//
// Deflate streams are read from least to most significant bit.
// An offset of 1 indicates that the least significant bit is skipped
// (i.e., only bits 7, 6, 5, 4, 3, 2, and 1 are read).
//
uint8_t deflate_bit_offset = 0;

//
// Base lengths for length codes (code 257 to 285).
// Code 257 corresponds to a copy of 3 bytes, etc.
//
uint32_t const deflate_length_offsets[] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
};

//
// Extra bits for length codes (code 257 to 285).
// Code 257 has no extra bits, code 265 has 1 extra bit
// (and indicates a length of 11 or 12 depending on its value), etc.
//
uint8_t const deflate_length_bits[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};

// can also be expressed as (index < 4 || index == 28) ? 0 : (index-4) >> 2

//
// Base distances for distance codes (code 0 to 29).
// Code 0 indicates a distance of 1, etc.
//
uint32_t const deflate_distance_offsets[] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385,
	513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

//
// Extra bits for distance codes (code 0 to 29).
// Code 0 has no extra bits, code 4 has 1 bit, etc.
///
uint8_t const deflate_distance_bits[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};

// can also be expressed as index < 2 ? 0 : (index-2) >> 1

//
// In block type 2 (dynamic huffman codes), the code lengths of literal/length
// and distance alphabet are themselves stored as huffman codes. To save space
// in case only a few code lengths are used, the code length codes are stored
// in the following order. This allows a few bits to be saved if some code
// lengths are unused and the unused code lengths are at the end of the list.
//
uint8_t const deflate_hclen_index[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

//
// Code lengths of the "code length" code (see above).
//
uint8_t deflate_hc_lengths[19];

//
// Code lengths of the literal/length and distance alphabets.
// up to 288 literal/length codes + up to 30 distance codes.
//
uint8_t deflate_lld_lengths[318];

#ifdef DEFLATE_WITH_LUT
uint32_t deflate_ll_codes[288];
uint32_t deflate_d_codes[30];
#endif

//
// Bit length counts and next code entries for Literal/Length alphabet.
// Combined with the code lengths in deflate_lld_lengths, these make up the
// Literal/Length alphabet. See the algorithm in RFC 1951 section 3.2.2 for
// details.
//
// Assumption: There are no more than 255 huffman codes with the same length.
// As the largest alphabet (the literal/length alphabet) contains just 288
// codes in total, this should be reasonable.
//
// These variables are also used for the huffman alphabet in dynamic huffman
// blocks.
//
uint8_t deflate_bl_count_ll[16];
uint32_t deflate_next_code_ll[16];

//
// Bit length counts and next code entries for Distance alphabet. Note that,
// even though there are just 30 different distance codes, individual
// codes may be up to 16 bits long.
//
uint8_t deflate_bl_count_d[16];
uint32_t deflate_next_code_d[16];

static uint32_t deflate_rev_word(uint32_t zword, uint8_t zbits)
{
	uint32_t ret = 0;
	uint32_t mask = 1;
    uint32_t rmask;
	for (rmask = 1 << (zbits - 1); rmask > 0; rmask >>= 1) {
		if (zword & rmask) {
			ret |= mask;
		}
		mask <<= 1;
	}
	return ret;
}

static uint32_t deflate_bitmask(uint8_t bit_count)
{
	return (1 << bit_count) - 1;
}

static uint32_t deflate_get_word()
{
	uint32_t ret = 0;
	ret |= (deflate_input_now[0] >> deflate_bit_offset);
	ret |= (uint32_t) deflate_input_now[1] << (8 - deflate_bit_offset);
	if (deflate_bit_offset) {
		ret |= (uint32_t) (deflate_input_now[2] & deflate_bitmask(deflate_bit_offset)) << (16 - deflate_bit_offset);
	}
	return ret;
}

static uint32_t deflate_get_bits(uint8_t num_bits)
{
	uint32_t ret = deflate_get_word();
	deflate_bit_offset += num_bits;
	while (deflate_bit_offset >= 8) {
		deflate_input_now++;
		deflate_bit_offset -= 8;
	}
	return ret & deflate_bitmask(num_bits);
}

#ifdef DEFLATE_WITH_LUT
static void deflate_build_alphabet(uint8_t * lengths, uint32_t size, uint8_t * bl_count, uint32_t * next_code, uint32_t * codes)
#else
static void deflate_build_alphabet(uint8_t * lengths, uint32_t size, uint8_t * bl_count, uint32_t * next_code)
#endif
{
	uint32_t i;
	uint32_t code = 0;
	uint32_t max_len = 0;
	for (i = 0; i < 16; i++) {
		bl_count[i] = 0;
	}

	for (i = 0; i < size; i++) {
		if (lengths[i]) {
			bl_count[lengths[i]]++;
		}
		if (lengths[i] > max_len) {
			max_len = lengths[i];
		}
	}

	for (i = 1; i <= max_len; i++) {
		code = (code + bl_count[i - 1]) << 1;
		next_code[i] = code;
	}

#ifdef DEFLATE_WITH_LUT
	uint8_t j = 0;
	code = 0;
	for (j = 1; j <= max_len; j++) {
		for (i = 0; i < size; i++) {
			if (lengths[i] == j) {
				codes[code++] = i;
			}
		}
	}
#endif
}

#ifdef DEFLATE_WITH_LUT
static uint32_t deflate_huff(uint32_t * codes, uint8_t * bl_count, uint32_t * next_code)
#else
//
// This function trades speed for low memory requirements. Instead of building
// an actual huffman tree (at a cost of about 650 Bytes of RAM), we iterate
// through the code lengths whenever we have found a huffman code.  This is
// very slow, but memory-efficient.
//
static uint32_t deflate_huff(uint8_t * lengths, uint32_t size, uint8_t * bl_count, uint32_t * next_code)
#endif
{
	uint32_t next_word = deflate_get_word();
#ifdef DEFLATE_WITH_LUT
	uint32_t code = 0;
#endif
    uint8_t num_bits;
	for (num_bits = 1; num_bits < 16; num_bits++) {
		uint32_t next_bits = deflate_rev_word(next_word, num_bits);
		if (bl_count[num_bits] && next_bits >= next_code[num_bits] && next_bits < next_code[num_bits] + bl_count[num_bits]) {
			deflate_bit_offset += num_bits;
			while (deflate_bit_offset >= 8) {
				deflate_input_now++;
				deflate_bit_offset -= 8;
			}
#ifdef DEFLATE_WITH_LUT
			return codes[code + (next_bits - next_code[num_bits])];
#else
			uint8_t len_pos = next_bits;
			uint8_t cur_pos = next_code[num_bits];
            uint32_t i;
			for (i = 0; i < size; i++) {
				if (lengths[i] == num_bits) {
					if (cur_pos == len_pos) {
						return i;
					}
					cur_pos++;
				}
			}
#endif
		} else {
#ifdef DEFLATE_WITH_LUT
			code += bl_count[num_bits];
#endif
		}
	}
	return 65535;
}

#ifdef DEFLATE_WITH_LUT
static int8_t deflate_huffman(uint32_t * ll_codes, uint32_t * d_codes)
#else
static int8_t deflate_huffman(uint8_t * ll_lengths, uint32_t ll_size, uint8_t * d_lengths, uint8_t d_size)
#endif
{
	uint32_t code;
	uint32_t dcode;
	while (1) {
#ifdef DEFLATE_WITH_LUT
		code = deflate_huff(ll_codes, deflate_bl_count_ll, deflate_next_code_ll);
#else
		code = deflate_huff(ll_lengths, ll_size, deflate_bl_count_ll, deflate_next_code_ll);
#endif
		if (code < 256) {
			if (deflate_output_now == deflate_output_end) {
				return DEFLATE_ERR_OUTPUT_LENGTH;
			}
			*deflate_output_now = code;
			deflate_output_now++;
		} else if (code == 256) {
			return 0;
		} else if (code == 65535) {
			return DEFLATE_ERR_HUFFMAN;
		} else {
			uint32_t len_val = deflate_length_offsets[code - 257];
			uint8_t extra_bits = deflate_length_bits[code - 257];
			if (extra_bits) {
				len_val += deflate_get_bits(extra_bits);
			}
#ifdef DEFLATE_WITH_LUT
			dcode = deflate_huff(d_codes, deflate_bl_count_d, deflate_next_code_d);
#else
			dcode = deflate_huff(d_lengths, d_size, deflate_bl_count_d, deflate_next_code_d);
#endif
			uint32_t dist_val = deflate_distance_offsets[dcode];
			extra_bits = deflate_distance_bits[dcode];
			if (extra_bits) {
				dist_val += deflate_get_bits(extra_bits);
			}
			while (len_val--) {
				if (deflate_output_now == deflate_output_end) {
					return DEFLATE_ERR_OUTPUT_LENGTH;
				}
				deflate_output_now[0] = *(deflate_output_now - dist_val);
				deflate_output_now++;
			}
		}
		if (deflate_input_now >= deflate_input_end - 4) {
			return DEFLATE_ERR_INPUT_LENGTH;
		}
	}
}

static int8_t deflate_uncompressed()
{
	if (deflate_bit_offset) {
		deflate_input_now++;
		deflate_bit_offset = 0;
	}
	uint32_t len = ((uint32_t) deflate_input_now[1] << 8) + deflate_input_now[0];
	uint32_t nlen = ((uint32_t) deflate_input_now[3] << 8) + deflate_input_now[2];
	if (len & nlen) {
		return DEFLATE_ERR_NLEN;
	}
	deflate_input_now += 4;
	if (deflate_input_now + len >= deflate_input_end) {
		return DEFLATE_ERR_INPUT_LENGTH;
	}
	if (deflate_output_now + len >= deflate_output_end) {
		return DEFLATE_ERR_OUTPUT_LENGTH;
	}
    uint32_t i;
	for (i = 0; i < len; i++) {
		*(deflate_output_now++) = *(deflate_input_now++);
	}
	return 0;
}

static int8_t deflate_static_huffman()
{
	uint32_t i;
	for (i = 0; i <= 143; i++) {
		deflate_lld_lengths[i] = 8;
	}
	for (i = 144; i <= 255; i++) {
		deflate_lld_lengths[i] = 9;
	}
	for (i = 256; i <= 279; i++) {
		deflate_lld_lengths[i] = 7;
	}
	for (i = 280; i <= 287; i++) {
		deflate_lld_lengths[i] = 8;
	}
	for (i = 288; i <= 288 + 29; i++) {
		deflate_lld_lengths[i] = 5;
	}

#ifdef DEFLATE_WITH_LUT
	deflate_build_alphabet(deflate_lld_lengths, 288, deflate_bl_count_ll, deflate_next_code_ll, deflate_ll_codes);
	deflate_build_alphabet(deflate_lld_lengths + 288, 29, deflate_bl_count_d, deflate_next_code_d, deflate_d_codes);
	return deflate_huffman(deflate_ll_codes, deflate_d_codes);
#else
	deflate_build_alphabet(deflate_lld_lengths, 288, deflate_bl_count_ll, deflate_next_code_ll);
	deflate_build_alphabet(deflate_lld_lengths + 288, 29, deflate_bl_count_d, deflate_next_code_d);
	return deflate_huffman(deflate_lld_lengths, 288, deflate_lld_lengths + 288, 29);
#endif
}

static int8_t deflate_dynamic_huffman()
{
	uint32_t hlit = 257 + deflate_get_bits(5);
	uint8_t hdist = 1 + deflate_get_bits(5);
	uint8_t hclen = 4 + deflate_get_bits(4);

    uint8_t i;
	for (i = 0; i < hclen; i++) {
		deflate_hc_lengths[deflate_hclen_index[i]] = deflate_get_bits(3);
	}
    //uint8_t i;
	for (i = hclen; i < sizeof(deflate_hc_lengths); i++) {
		deflate_hc_lengths[deflate_hclen_index[i]] = 0;
	}

#ifdef DEFLATE_WITH_LUT
	deflate_build_alphabet(deflate_hc_lengths, sizeof(deflate_hc_lengths), deflate_bl_count_ll, deflate_next_code_ll, deflate_ll_codes);
#else
	deflate_build_alphabet(deflate_hc_lengths, sizeof(deflate_hc_lengths), deflate_bl_count_ll, deflate_next_code_ll);
#endif

	uint32_t items_processed = 0;
	while (items_processed < hlit + hdist) {
#ifdef DEFLATE_WITH_LUT
		uint8_t code = deflate_huff(deflate_ll_codes, deflate_bl_count_ll, deflate_next_code_ll);
#else
		uint8_t code = deflate_huff(deflate_hc_lengths, sizeof(deflate_hc_lengths), deflate_bl_count_ll, deflate_next_code_ll);
#endif
        uint8_t i;
		if (code == 16) {
			uint8_t copy_count = 3 + deflate_get_bits(2);
            //uint8_t i;
			for (i = 0; i < copy_count; i++) {
				deflate_lld_lengths[items_processed] = deflate_lld_lengths[items_processed - 1];
				items_processed++;
			}
		} else if (code == 17) {
			uint8_t null_count = 3 + deflate_get_bits(3);
            //uint8_t i;
			for (i = 0; i < null_count; i++) {
				deflate_lld_lengths[items_processed] = 0;
				items_processed++;
			}
		} else if (code == 18) {
			uint8_t null_count = 11 + deflate_get_bits(7);
            //uint8_t i;
			for (i = 0; i < null_count; i++) {
				deflate_lld_lengths[items_processed] = 0;
				items_processed++;
			}
		} else {
			deflate_lld_lengths[items_processed] = code;
			items_processed++;
		}
	}

#ifdef DEFLATE_WITH_LUT
	deflate_build_alphabet(deflate_lld_lengths, hlit, deflate_bl_count_ll, deflate_next_code_ll, deflate_ll_codes);
	deflate_build_alphabet(deflate_lld_lengths + hlit, hdist, deflate_bl_count_d, deflate_next_code_d, deflate_d_codes);
	return deflate_huffman(deflate_ll_codes, deflate_d_codes);
#else
	deflate_build_alphabet(deflate_lld_lengths, hlit, deflate_bl_count_ll, deflate_next_code_ll);
	deflate_build_alphabet(deflate_lld_lengths + hlit, hdist, deflate_bl_count_d, deflate_next_code_d);
	return deflate_huffman(deflate_lld_lengths, hlit, deflate_lld_lengths + hlit, hdist);
#endif
}

int32_t inflate(const unsigned char *input_buf, uint32_t input_len, unsigned char *output_buf, uint32_t output_len)
{
	deflate_input_now = input_buf;
	deflate_input_end = input_buf + input_len;
	deflate_bit_offset = 0;

	deflate_output_now = output_buf;
	deflate_output_end = output_buf + output_len;

	while (1) {
		uint8_t block_type = deflate_get_bits(3);
		uint8_t is_final = block_type & 0x01;
		int8_t ret;

		block_type >>= 1;

		switch (block_type) {
		case 0:
			ret = deflate_uncompressed();
			break;
		case 1:
			ret = deflate_static_huffman();
			break;
		case 2:
			ret = deflate_dynamic_huffman();
			break;
		default:
			return DEFLATE_ERR_BLOCK;
		}

		if (ret < 0) {
			return ret;
		}

		if (is_final) {
			return deflate_output_now - output_buf;
		}
	}
}

int32_t inflate_zlib(const unsigned char *input_buf, uint32_t input_len, unsigned char *output_buf, uint32_t output_len)
{
	if (input_len < 4) {
		return DEFLATE_ERR_INPUT_LENGTH;
	}
	uint8_t zlib_method = input_buf[0] & 0x0f;
	uint8_t zlib_flags = input_buf[1];

	if (zlib_method != 8) {
		return DEFLATE_ERR_METHOD;
	}

	if (zlib_flags & 0x20) {
		return DEFLATE_ERR_FDICT;
	}

	if ((((uint32_t) input_buf[0] << 8) | input_buf[1]) % 31) {
		return DEFLATE_ERR_FCHECK;
	}

	int32_t ret =
	    inflate(input_buf + 2, input_len - 2, output_buf, output_len);

#ifdef DEFLATE_CHECKSUM
	if (ret >= 0) {
		uint32_t deflate_s1 = 1;
		uint32_t deflate_s2 = 0;

		deflate_output_end = deflate_output_now;
		for (deflate_output_now = output_buf; deflate_output_now < deflate_output_end; deflate_output_now++) {
			deflate_s1 = ((uint32_t) deflate_s1 + (uint32_t) (*deflate_output_now)) % 65521;
			deflate_s2 = ((uint32_t) deflate_s2 + (uint32_t) deflate_s1) % 65521;
		}

		if (deflate_bit_offset) {
			deflate_input_now++;
		}

		if ((deflate_s2 != (((uint32_t) deflate_input_now[0] << 8) | (uint32_t) deflate_input_now[1]))
		    || (deflate_s1 != (((uint32_t) deflate_input_now[2] << 8) | (uint32_t) deflate_input_now[3]))) {
			return DEFLATE_ERR_CHECKSUM;
		}
	}
#endif

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{

  if (argc < 2 || argc > 3)
    {
	printf("\n");
    printf("zlistbas <v%s> by Rafael Molina: \n", PROG_VER);
	printf("\n");
    printf("%s [option] file[.tzx|.tap|.sp|.sna|.z80|.bas|.o|.p|.p81|.t81] \n", "Usage: zlistbas");
    printf("%s [option] file[.rzx] \n", "               ");	   
	printf("        cc0      removed  colour code format  \n");	  
	printf("       [cc1]     zmakebas colour code format: \\{nn}\\{n} \n");
	printf("        cc2      bas2tap  attrib str  format: {attrstr nn} \n");	
	printf("        cc3      basinC1  colour code format: \\#0nn\\#00n \n");
	printf("        cc4      basinC2  colour code format: \\{An} A=i|p|f|b|v|o \n");
	printf("        cc5      generic  colour code format: \";CHR$ nn;CHR$ n;\" \n");
    exit (0);
    }

  if (argc == 2) k = 1;
  else{
    strcpy (buf, argv[1]);
    if (!strcmp(buf, "cc0")) colorcode=cc0;
    else if (!strcmp(buf, "cc1")) colorcode=cc1;	
    else if (!strcmp(buf, "cc2")) colorcode=cc2;
    else if (!strcmp(buf, "cc3")) colorcode=cc3;
    else if (!strcmp(buf, "cc4")) colorcode=cc4;
    else if (!strcmp(buf, "cc5")) colorcode=cc5;		
    else Error ("option is not valid!");	
    k = 2;
  }
  strcpy (buf, argv[k]);
  if ((fIn = fopen (argv[k], "rb")) == NULL) 
    Error ("Can't read file!");

  flen = FileLength (fIn);

  if ((mem = (unsigned char *) malloc (flen)) == NULL) 
    Error ("Not enough memory to load input file!");

  if (fread (mem, 1, 10, fIn) != 10)
    Error ("Read error!");

  const unsigned char TZXSTART[]="ZXTape!";
  const unsigned char RZXSTART[]="RZX!";  
  const unsigned char TAPSTART[]={0x13,0x00};
  const unsigned char SPSTART[]="SP";  
  const unsigned char SNAEXT[]=".SNA";
  const unsigned char Z80EXT[]=".Z80";
  const unsigned char BASSTART[]="PLUS3DOS";
  const unsigned char OEXT[]=".O";  
  const unsigned char PEXT[]=".P";
  const unsigned char P81EXT[]=".P81";
  const unsigned char T81START[]="EO81";

  if (!hdrcmp((char *)argv[k] +strlen(argv[k]) -4, (char *)SNAEXT, 4)) fformat= SNA;
  else if (!hdrcmp((char *)argv[k] +strlen(argv[k]) -4, (char *)Z80EXT, 4)) fformat= Z80;
  else if (!hdrcmp((char *)argv[k] +strlen(argv[k]) -2, (char *)OEXT, 2)) fformat= O;  
  else if (!hdrcmp((char *)argv[k] +strlen(argv[k]) -2, (char *)PEXT, 2)) fformat= P;
  else if (!hdrcmp((char *)argv[k] +strlen(argv[k]) -4, (char *)P81EXT, 4)) fformat= P81;
  else if (!hdrcmp((char *)mem, (char *)T81START, 4)) fformat= T81;  
  else if (!hdrcmp((char *)mem, (char *)SPSTART, 2)) fformat= SP;
  else if (!hdrcmp((char *)mem, (char *)TZXSTART, 7)) fformat= TZX;
  else if (!hdrcmp((char *)mem, (char *)RZXSTART, 4)) fformat= RZX;  
  else if (!hdrcmp((char *)mem, (char *)TAPSTART, 2)) fformat= TAP;
  else if (!hdrcmp((char *)mem, (char *)BASSTART, 8)) fformat= BAS;
  else {free(mem); Error ("Unknown ZX file format!");}

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
	case RZX: 
			 RZXPROC();
			 break;	   	   	    
	case SP: 
			 SPPROC();
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
	case O: 
			 OPROC();
			 break;	   	   	    
	case P: 
			 PPROC();
			 break;
	case P81: 
			 P81PROC();
			 break;
	case T81: 
			 T81PROC();
			 break;	   	   	       	   	    
	}
	
  //system("pause");
  fclose (fIn);
  //free (mem); 
    
  return (0);
}

int hdrcmp (char *mem, char *hdrstart, int len)
{

  int i= 0;
  while (i< len)
  {
    //printf("mem[%d]=%d hdrstart[%d]=%d\n", i, mem[i], i, hdrstart[i]);
    if (mem[0] == '.') mem[i]= toupper(mem[i]);
	if (mem[i] != hdrstart[i]) return 1;

	i++;
  }

  return 0;
}

/* Changes the File Extension of String *str to *ext */
void ChangeFileExtension (char *str, char *ext)
{
  int n;
  
  n = strlen(str); 

  while (str[n] != '.') 
    n--;

  str[n]= '_';
  str[n +1]= rzxcnt + 48;
  str[n +2]= '.';
  str[n +3]= 0;
  strcat(str, ext);
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

/*////////////////////////////////////////////////////////////////////////////
  DeTokenizeP()

  'In' should point to a string containing tokenized ZX81 text.
  'LineLen' should contain the length of the Input data in characters.
  'Out' This is the output expanded ascii text
////////////////////////////////////////////////////////////////////////////*/
int DeTokenizeP(int linelen)
{
int f, inQuotes = 0;
unsigned char c, keyword = LineData[0];
char *x;

for (f = 0; f < linelen - 1; f++)
    {
    c = LineData[f]; /* Character code  */
    x = charset[c]; /* Translated code */

    if ( (keyword != REM_code) && (c == QUOTE_code) )
        inQuotes = !inQuotes;

    if ( (keyword != REM_code) && (c == NUM_code) )
        f += 5;  /* avoid inline FP numbers - but ok for REMs */

    else if ( (keyword == REM_code && f > 0) || inQuotes)
    {

		if (colorcode == 1) {
	        if ((!onlyFirstLineREM || inFirstLineREM) && ((strcmp(x, NAK) == 0) || ((strlen(x) >1) && (x[0] != '\\') && (x[0]!='`'))) )
	            printf("\\{%d}", c); // Print escaped as char code
	        else
	            printf("%s", x); // Print translated char
		}
		else if (colorcode == 2) {
	        if (inFirstLineREM) printf("\\%02X", c); // Eightyone b81 format
	        else printf("%s", x);
		}
		else if (colorcode == 3) {
	        if (inFirstLineREM) printf("\\{0x%02X}", c);
	        else printf("%s", x);
		}
		else if (colorcode == 4) {
	        if ((!onlyFirstLineREM || inFirstLineREM) && ((strcmp(x, NAK) == 0) || ((strlen(x) >1) && (x[0] != '\\') && (x[0]!='`'))) ) printf("\\%02X", c);
	        else printf("%s", x);
		}
		else if (colorcode == 5) {
	        if ((!onlyFirstLineREM || inFirstLineREM) && ((strcmp(x, NAK) == 0) || ((strlen(x) >1) && (x[0] != '\\') && (x[0]!='`'))) ) printf("\\{0x%02X}", c);
	        else printf("%s", x);
		}	     	   
		else {	// colorcolode == 0
	        if (inFirstLineREM) printf("\\{%03d}", c);
	        else printf("%s", x);
		}	    		
	}	 	 
    else
        printf("%s", x); // Print translated char
    }
printf("\n");
}

/*////////////////////////////////////////////////////////////////////////////
  DeTokenize()

  'In' should point to a string containing tokenized ZX Spectrum text.
  'LineLen' should contain the length of the Input data in characters.
  'Out' This is the output expanded ascii text
////////////////////////////////////////////////////////////////////////////*/
int DeTokenize(unsigned char *In,int LineLen,unsigned char *Out)
{
    int i = 0, o = 0;

    char cc1f[]= "\\{nn}\\{n}";
	char cc3f[]= "\\#0nn\\#00n";
	char cc2i[]= "{INK  }", cc2p[]= "{PAPER  }", cc2f[]= "{FLASH  }", cc2b[]= "{BRIGHT  }", cc2v[]= "{INVERSE  }", cc2o[]= "{OVER  }";
	char cc4f[]= "\\{An}";
	char cc5f[]= "\";CHR$ nn;CHR$ n;\"";
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
			  case 2:  cc=cc2i; cc[5]=48+In[i+1] ;ConCat(Out,&o, cc); break;
	          case 3:  cc=cc3f; cc[3]='1';cc[4]='6';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='i';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='1';cc[8]='6';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   	    		
            i++;      /* embedded INK */
            break;
        case 17:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='7';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
			  case 2:  cc=cc2p; cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;	   	     
	          case 3:  cc=cc3f; cc[3]='1';cc[4]='7';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='p';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='1';cc[8]='7';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   	    	
            i++;      /* embedded PAPER */
            break;
        case 18:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='8';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
			  case 2:  cc=cc2f; cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;	   	     
	          case 3:  cc=cc3f; cc[3]='1';cc[4]='8';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='f';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='1';cc[8]='8';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;
	        }	   
            i++;      /* embedded FLASH */
            break;
        case 19:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='1';cc[3]='9';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
			  case 2:  cc=cc2b; cc[8]=48+In[i+1] ;ConCat(Out,&o, cc); break;	   	     
	          case 3:  cc=cc3f; cc[3]='1';cc[4]='9';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='b';cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='1';cc[8]='9';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;	   	    
	        }	   
            i++;      /* embedded BRIGHT */
            break;
        case 20:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='2';cc[3]='0';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
			  case 2:  cc=cc2v; cc[9]=48+In[i+1] ;ConCat(Out,&o, cc); break;	   	     
	          case 3:  cc=cc3f; cc[3]='2';cc[4]='0';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='v';if (In[i+1]) cc[3]='i'; else cc[3]='n' ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='2';cc[8]='0';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
			  default: break;			  
	        }	   
            i++;      /* embedded INVERSE */
            break;
        case 21:
		    switch (colorcode) {
	          case 1:  cc=cc1f; cc[2]='2'; cc[3]='1';cc[7]=48+In[i+1] ;ConCat(Out,&o, cc); break;
			  case 2:  cc=cc2o; cc[6]=48+In[i+1] ;ConCat(Out,&o, cc); break;	   	     
	          case 3:  cc=cc3f; cc[3]='2'; cc[4]='1';cc[9]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 4:  cc=cc4f; cc[2]='o'; cc[3]=48+In[i+1] ;ConCat(Out,&o, cc);break;
	          case 5:  cc=cc5f; cc[7]='2'; cc[8]='1';cc[15]=48+In[i+1] ;ConCat(Out,&o, cc);break;
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

int TZXPROC()
{
    pos = 0x0A;
    j = pos;
    while (pos < flen){
		 //printf("pos=0x%X mem[pos]=0x%X\n", pos, mem[pos]);
	     switch (mem[pos])
	        {
	        case 0x10:
			    ProgLen = Get2(&mem[pos +1 +2 +2 +0x10]);
				len = Get2(&mem[pos +1 +2 +2 +0x13 +1 +2]);	   //Data Block length
			    while ((5 +0x13 +5 +1 +j <  pos +1 +2 +2 +0x13 +1 +2 +2 +ProgLen) && (mem[pos +5 +1]== 0)) {
					LineNum = 256*mem[5 +0x13 +5 +1 +j] + mem[5 +0x13 +5 +2 +j];
			        if (LineNum > 16384) break; //se salta la zona de vars tras programa
			
			        LineLen = mem[5 +0x13 +5 +3 +j] + 256*mem[5 +0x13 +5 +4 +j];
                    if (LineLen > ProgLen -5) LineLen= ProgLen -5;	  // flag(1) +LineNum(2) +LineLen(2)
			
					memcpy(LineData,mem+5 +0x13 +5 +5 +j, LineLen);
			        LineData[LineLen]= 0; // Terminate the line data
					
			        DeTokenize(LineData, LineLen, LineText);
					printf("%d%s\n", LineNum, LineText);
					
					j= j +2 +2 +LineLen;      
			    }
				if (mem[pos +5 +1]== 0) printf("\n");
				pos= j= pos +1 +2 +2 +0x13 +1 +2 +2 +len; // header block length= id(1) +pause(2) +tzx block length(2) +0x13
				break;    	 	 	 	 	 	 	 	  // data block length=  id(1) +pause(2)  +tzx block length(2) +len	   
		    case 0x11:
			    if ((Get3(&mem[pos +1 +13 +2]) == 0x13) && (mem[pos +19 +1]== 0)) {
				    ProgLen = Get2(&mem[pos +1 +13 +2 +3 +0x10]);
					len = Get3(&mem[pos +1 +13 +2 +3 +0x13 +1 +13 +2]);	   //Data Block length
				    while ((19 +0x13 +19 +1 + j <  pos +1 +13 +2 +3 +0x13 +1 +13 +2 +3 +ProgLen) && (mem[pos +19 +1]== 0)) {
						LineNum = 256*mem[19 +0x13 +19 +1 +j] + mem[19 +0x13 +19 +2 +j];
				        if (LineNum > 16384) break; //se salta la zona de vars tras programa
				
				        LineLen = mem[19 +0x13 +19 +3 +j] + 256*mem[19 +0x13 +19 +4 +j];
						if (LineLen > ProgLen -5) LineLen =ProgLen -5;	  // flag(1) +LineNum(2) +LineLen(2)
						memcpy(LineData,mem+19 +0x13 +19 +5 +j, LineLen);
				        LineData[LineLen]= 0; // Terminate the line data
						
				        DeTokenize(LineData, LineLen, LineText);
						printf("%d%s\n",LineNum, LineText);
						j= j +2 +2 +LineLen;      
				    }
					if (mem[pos +19 +1]== 0) printf("\n");
					pos= pos +1 +13 +2 +3 +0x13 +1 +13 +2 +3 +len; // header block length= id(1) +extra(13) +pause(2) +tzx length(3) +0x13
					j= pos;	 	 	 	 	 	 	 	             // data block length=  id(1) +extra (13) +pause(2)  +tzx length(3) + len
                }
				else {
				    pos =j= pos +1 +13 +2 +3 +Get3(&mem[pos +1 +13 +2]);
				}
			    break;
			case 0x12:
			    pos= j= pos +1 +2 +2; break;			
			case 0x13:
			    pos= j= pos +1 +1 +2*mem[pos +1]; break;
			case 0x14:
			    pos= j= pos +1 +2 +2 +1 +2 +3 +Get3(&mem[pos +1 +2 +2 +1 +2]); break;
			case 0x15:
			    pos= j= pos +1 +2 +2 +1 +3 +Get3(&mem[pos +1 +2 +2 +1]); break;
			case 0x18:
			    pos= j= pos +1 +4 +Get4(&mem[pos +1]); break;
		    case 0x19:
				len = Get4(&mem[pos +1]);	//Data Block length
				int delta= 0x58 +zlen(mem +pos +1 +4 +0x58);
				int inDFILEpos= pos +1 +4 +delta +mem[pos +1 +4 +delta +3] + 256*mem[pos +1 +4 +delta +4] -16393;
			    while (1 +4 +delta +0x74 +j <  inDFILEpos) { //pos +1 +4 +len
					LineNum = 256*mem[1 +4 + delta +0x74 +j] + mem[1 +4 + delta +0x74 +1 +j];
			        if (LineNum > 16384) break; //se salta la zona de vars tras programa
			        LineLen = mem[1 +4 + delta +0x74 +2 +j] + 256*mem[1 +4 + delta +0x74 +3 +j];
					//if (LineLen > ProgLen -5) LineLen =ProgLen -5;	// flag(1) +LineNum(2) +LineLen(2)

					memcpy(LineData,mem +1 +4 + delta +0x74 +4 +j , LineLen);
			        LineData[LineLen]= 0; // Terminate the line data
					
                    printf("%4d", LineNum);
                    DeTokenizeP(LineLen);
                    inFirstLineREM = 0;
					j= j +2 +2 +LineLen;      
			    }
				printf("\n");
				pos= j= pos +1 +4 +len;
			    break;
			case 0x20:
			    pos= j= pos +1 +2; break;
			case 0x21:
			    pos= j= pos +1 +1 +mem[pos +1]; break;
			case 0x22:
			    pos= j= pos +1; break;
			case 0x23:
			    pos= j= pos +1 +2; break;
			case 0x24:
			    pos= j= pos +1 +2; break;
			case 0x25:
			    pos= j= pos +1; break;
			case 0x26:
			    pos= j= pos +1 +2 +2*Get2(&mem[pos +1]); break;
			case 0x27:
			    pos= j= pos +1; break;
			case 0x28:
			    pos= j= pos +1 +2 +Get2(&mem[pos +1]); break;
			case 0x2A:
			    pos= j= pos +1 +4; break;
			case 0x2B:
			    pos= j= pos +1 +4 +1; break;	  	  	  
			case 0x30:
			    pos= j= pos +1 +1 +mem[pos +1]; break;
			case 0x31:
			    pos= j= pos +1 +1 +1 +mem[pos +1 +1]; break;
			case 0x32:
			    pos= j= pos + 1 +2 +Get2(&mem[pos +1]); break;
			case 0x33:
			    pos= j= pos +1 +1 +3*mem[pos +1 +1]; break;
			case 0x35:
			    pos= j= pos + 1 +0x10 +4 +Get4(&mem[pos +1 +0x10]); break;
			case 0x4B:
			    pos= j= pos +1 +4 +Get4(&mem[pos +1]); break;
			case 0x5A:
			    pos= j= pos +1 +9; break;
		    default:
                fprintf(stderr,"tzx block 0x%X not supported yet!\n", mem[pos]);
                return(2);	  	  
		        break;
		    }
    }
	free(mem);	 
}

int RZXPROC()
{
    int uslen;
    zmem= mem;
//    if ((zmem = (unsigned char *) malloc (flen)) == NULL) 
//       Error ("Not enough memory to load input file!");     
//    memcpy(zmem, mem, flen);
//	  free(mem);
				   	    
    pos= 0x0A;
    j= pos;
    while (pos < flen){
		 //printf("pos=0x%X zmem[pos]=0x%X\n", pos, zmem[pos]);
	     switch (zmem[pos])
	        {
		    case 0x30:
				len= Get4(&zmem[pos +1]) -1 -4;	  //Data Block length
				uslen= Get4(&zmem[pos +0x0D]);
	            if ((mem = (unsigned char *) malloc (uslen)) == NULL) 
                   Error ("Not enough memory to load uncompressed data!");
				
				//printf("len=%d uslen=%d\n",len, uslen);
			    //printf("zpos5=%d %c %c %c\n", *(zmem+ pos +5), *(zmem +pos +9), *(zmem +pos +10), *(zmem +pos +11));
				if (zmem[pos +5] & 0x02)
				   inflate_zlib(zmem +pos +0x11, len -0x0C, mem, uslen);
				else
				   memcpy(mem, zmem +pos +0x11, uslen);

                flen= uslen;				   
                if (!strcasecmp(zmem +pos +9, "Z80")) Z80PROC();
				else if (!strcasecmp(zmem +pos +9, "SNA")) SNAPROC();

				ChangeFileExtension(buf, zmem +pos +9);FILE *fho=fopen(buf,"wb");fwrite(mem, 1, uslen, fho);fclose(fho); rzxcnt++;

                //free(mem);
				pos= j= pos +1 +4 +len;
				break;
		    default:
			    len = Get4(&zmem[pos +1]) -1 -4;
                pos= j= pos +1 +4 +len;
		        break;
		    }
    }
	free(zmem);	  
}


void TAPPROC()
{
	pos= 0;
	j= pos;
    while (pos < flen){
          ProgLen= Get2(&mem[pos +2 +0x10]);
          len= Get2(&mem[pos + 0x15]);	       //Data Block length
		  while ((0x18 +j < pos +2 +0x13 +2 +ProgLen) && (mem[pos + 0x03] == 0)) {

				LineNum = 256*mem[0x18 + j] +mem[0x19 + j];
				if (LineNum > 16384) break;

				LineLen = mem[0x1A +j] +256*mem[0x1B +j];
                if (LineLen > ProgLen -5) LineLen= ProgLen -5;	  // flag(1) +LineNum(2) +LineLen(2)
				
				memcpy(LineData,mem +0x1C +j ,LineLen);
		        LineData[LineLen]= 0; // Terminate the line data
		
		        DeTokenize(LineData, LineLen, LineText);
		
                printf("%d%s\n", LineNum, LineText);
				j= j +2 +2 +LineLen;
				
          }
		  if (mem[pos +0x03]==0) printf("\n");
		  pos = pos +2 +0x13 +2 +len; // header block length= 2 +0x13 and data block length= 2 +len
		  j= pos;
    }
    free(mem);
}

int SPPROC()
{
	const int HDRLEN= 38;
	int startAddr= Get2(&mem[4]);
    len= Get2(&mem[2]); if (len== 0) {len= 65536; startAddr=0;}
	
    // Get the [PROG] system variable (PEEK 23635+256*PEEK 23636)
	pos= 23635 -startAddr +HDRLEN;
    ProgAddr= Get2(&mem[pos]);

    if ((ProgAddr < 23296) ||(ProgAddr > 65530)){
        fprintf(stderr,"Invalid [PROG] in system in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    // Get the [VARS] system variable (PEEK 23627+256*PEEK 23628)
	pos= 23627 -startAddr +HDRLEN;
    VarsAddr= Get2(&mem[pos]);

    if ((VarsAddr < 23296) ||(VarsAddr > 65530)){
        fprintf(stderr,"Invalid [VARS] in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    ProgLen= VarsAddr -ProgAddr;	//ProgLen= [VARS] -[PROG] = VarsAddr - ProgAddr;
    //printf("ProgAddr=%d VarsAddr=%d ProgLen=%d\n", ProgAddr, VarsAddr, ProgLen);
	
    // seek to PROG area
	flen= VarsAddr -startAddr +HDRLEN;
	pos= ProgAddr -startAddr +HDRLEN;
    while (pos < flen) {	
        LineNum = 256*mem[pos] +mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen= mem[pos +2] +256*mem[pos +3];
		if (LineLen > flen -pos -4) LineLen= flen -pos -4;

		memcpy(LineData, mem +pos +4, LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        DeTokenize(LineData, LineLen, LineText);
        printf("%d%s\n", LineNum, LineText);
		
		pos= pos +2 +2 +LineLen; 	       
    }
	printf("\n");
	free(mem);
	return(0);

}

int SNAPROC()
{
    const int HDRLEN= 27;
	const int STARTADDR= 16384;
    // All 48K snapshots are 49179 bytes, check to ensure our .sna is at least this long
    if (flen < 49179) {
        fprintf(stderr,".SNA files should be at least 49179 bytes long, the input file is %d bytes\n",flen);
        return(3);
    }

    // Get the [PROG] system variable (PEEK 23635+256*PEEK 23636)
	pos= 23635 -STARTADDR +HDRLEN;
    ProgAddr= Get2(&mem[pos]);

    if ((ProgAddr < 23296) ||(ProgAddr > 65530)){
        fprintf(stderr,"Invalid [PROG] in system in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    // Get the [VARS] system variable (PEEK 23627+256*PEEK 23628)
	pos= 23627 -STARTADDR +HDRLEN;
    VarsAddr= Get2(&mem[pos]);

    if ((VarsAddr < 23296) ||(VarsAddr > 65530)){
        fprintf(stderr,"Invalid [VARS] in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    ProgLen= VarsAddr -ProgAddr;	//ProgLen= [VARS] - [PROG] = VarsAddr - ProgAddr;
    //printf("ProgAddr=%d VarsAddr=%d ProgLen=%d\n", ProgAddr, VarsAddr, ProgLen);
	
    // seek to PROG area
	flen= VarsAddr -STARTADDR +HDRLEN;
	pos= ProgAddr -STARTADDR +HDRLEN;
    while (pos < flen) {	
        LineNum = 256*mem[pos] +mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = mem[pos +2] +256*mem[pos +3];
		if (LineLen > flen -pos -4) LineLen= flen -pos -4;

		memcpy(LineData, mem +pos +4 ,LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        DeTokenize(LineData, LineLen, LineText);
        printf("%d%s\n",LineNum,  LineText);
		
		pos= pos +2 +2 +LineLen; 	       
    }
	printf("\n");
	free(mem);
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
byte ver, j, rommod, verbose= 0;
byte *buf1, *buf2, *buf3;


unsigned short int unpack(unsigned char *inp, unsigned char *outp, unsigned short int size)
{
    register unsigned short int incount=0,outcount=0;
    unsigned short int i;
    unsigned char j;
    do {
       if ((inp[0] == 0xED)&&(inp[1] == 0xED)) {
            i= inp[2];
            j= inp[3];
            inp+=4;
            incount+=4;
            outcount+=i;
            for (;i!=0;i--) *(outp++)=j;
       } else {
         *(outp++)=*(inp++);
         incount++;
         outcount++;
       }
    } while (outcount < size);
    if (outcount!= size) incount=0;
    return (incount);
}
     pos= 0;
     //fread(&buffer,1,30,fin);
	 ver= 0;
	 if (mem[6]==0) {		// pc =0 for z80 ver >=2
		 //fread(&(buffer.length), 1, 2, fin); // z80 version
		 len= Get2(&mem[30]);
		 switch (len) {
	        case 21:
	        case 23:
			     //printf ("(Version 2.0 .Z80 file)\n\n");
	             ver= 2;
	             break;
	        case 54:
	             //printf ("(Version 3.0 .Z80 file)\n\n");
	             ver= 3;
	             break;
            case 55:
                 //printf ("(Plus3 .Z80 file)\n\n");
                 ver= 3;
                 break;	     	 	 	  
	        default:
		         //printf ("\nUnsupported version of .Z80 file, or .Z80 file corrupt\n\n");
		         return(1);
		    }
			//fread(&(buffer.rpc2), 1, buffer.length, fin);
			pos= 32 +len;
     } else {
	     //printf ("(Version 1.45 or earlier .Z80 file)\n\n");
		 pos= 30;  
     }
	
    if (ver<2) {
         buf1= (unsigned char*)malloc(49152L+256);
         buf3= (unsigned char*)malloc(49152L+256);
		 //i=fread(buf1,1,49152L+256,fIn);	      

		 //if (buffer.rr_bit7&32) unpack(buf1,buf2,49152L);
		 if (mem[12]&32) {
             memcpy(buf1, mem +pos ,flen -pos);
		     unpack(buf1, buf3, 49152L);
			 flen= 49152;         //Block of data is compressed, bit5 ON
		 }
		 else {
		     memcpy(buf3, mem +pos ,flen -pos);
		 }

		 //printblock(buf3,49152L,16384)	 	 // buf3
		 pos= flen;
		 free(buf1);
		 
	} else {
	     buf1= (unsigned char*)malloc(16384+256);
	     buf2= (unsigned char*)malloc(16384+256);
         buf3= (unsigned char*)malloc(49152L+256);
		 	    
	     //i=fread(&blockhdr,1,3,fin);			// 3 bytes: 2 bytes for length and 1 byte for pagenumber
	     while (pos < flen) {
		    len= Get2(&mem[pos]);
            //printf ("Page nr: %2d      Block length: %d\n", mem[pos +2], len);
            //i=fread(buf1,1,blockhdr.length,fin);
			
		    if (len == 65535) {
			    len= 16384;
				memcpy(buf2, mem +pos +3, len);
		    }					
	 	    else {
				memcpy(buf1, mem +pos +3, len);
				unpack(buf1, buf2, 16384);
			}
			
			switch (mem[pos +2])
			  {
			  case 4: memcpy(buf3 + 16384, buf2, 16384); break; // page 4: 8000-bfff
			  case 5: memcpy(buf3 + 32768, buf2, 16384); break; // page 5: c000-ffff
			  case 8: memcpy(buf3, buf2, 16384); break;         // page 8: 4000-7fff
			  }
			
	 	    //printblock(buf2,16384,0)
			
		    //i=fread(&blockhdr,1,3,fin);
			pos= pos +3 +len;
	     }
		 flen= 49152;
		 free(buf1);
		 free(buf2);
		 //printblock(buf3,49152,0)
    }

    const int HDRLEN= 0;
    const int STARTADDR= 16384;
	
    // Get the [PROG] system variable (PEEK 23635+256*PEEK 23636)
	pos= 23635 -STARTADDR +HDRLEN;
    ProgAddr= Get2(&buf3[pos]);

    if ((ProgAddr < 23296) || (ProgAddr > 65530)){
        fprintf(stderr,"Invalid [PROG] in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    // Get the [VARS] system variable (PEEK 23627+256*PEEK 23628)
	pos= 23627 -STARTADDR +HDRLEN;
    VarsAddr= Get2(&buf3[pos]);

    if ((VarsAddr < 23296) ||(VarsAddr > 65530)){
        fprintf(stderr,"Invalid [VARS] in system variable area, does not contain a BASIC program.\n");
        return(4);
    }

    ProgLen= VarsAddr -ProgAddr;	//ProgLen= [VARS] - [PROG] = VarsAddr - ProgAddr;
    //printf("ProgAddr=%d VarsAddr=%d ProgLen=%d\n", ProgAddr, VarsAddr, ProgLen);
	
    // seek to PROG area
	pos= ProgAddr -STARTADDR +HDRLEN;
	flen= VarsAddr -STARTADDR +HDRLEN;
    while (pos < flen) {	
        LineNum = 256*buf3[pos] +buf3[pos +1];
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = buf3[pos +2] +256*buf3[pos +3]; //printf("LineNum=%d LineLen=%d\n", LineNum, LineLen);
        if (LineLen > flen -pos -4) LineLen= flen -pos -4;

		memcpy(LineData, buf3 +pos +4, LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        DeTokenize(LineData, LineLen, LineText);
        printf("%d%s\n", LineNum, LineText);
		
		pos= pos +2 +2 +LineLen; 	       
    }
	printf("\n");
	free(buf3);
	free(mem);
	return(0);	      	   


}

void BASPROC()
{
    const int HDRLEN =128;
	
	flen= Get4(&mem[0x0B]);
	pos= HDRLEN;
    while ((pos < flen) && (mem[0x0F]==0)) {

		LineNum = 256*mem[pos] +mem[pos +1];
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

        LineLen = mem[pos +2] +256*mem[pos +3];
        if (LineLen > flen -pos -4) flen= ProgLen - pos -4;

		memcpy(LineData, mem +pos +4, LineLen);
        LineData[LineLen]= 0; // Terminate the line data
		
        DeTokenize(LineData, LineLen, LineText);
		printf("%d%s\n", LineNum, LineText);
		
		pos= pos +2 +2 +LineLen;      
    }
	if (mem[0x0F]==0) printf("\n");
	free(mem);
}

int OPROC()
{
    charset = charset_zx80zmb;
    const int HDRLEN= 0;
	const int STARTADDR= 16384;	// 0x4000

    ProgAddr = 16424;

    // Get the [VARS] system variable (PEEK 16392+256*PEEK 16393)
	pos= 16392 -STARTADDR +HDRLEN;
    VarsAddr= Get2(&mem[pos]);

    // seek to PROG area
	flen= VarsAddr -STARTADDR +HDRLEN;
	pos= ProgAddr -STARTADDR +HDRLEN;
    inFirstLineREM = (mem[pos +2] == REM_zx80code);
    while (pos < flen) {	
        LineNum= 256*mem[pos] +mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen= zlinelen(mem +pos +2);
		memcpy(LineData, mem +pos +2 ,LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        printf("%4d", LineNum);
        DeTokenizeP(LineLen);
        inFirstLineREM = 0;
			
		pos= pos +2 +LineLen; 	        
    }
	printf("\n");
	free(mem);
	return(0);

}

int PPROC()
{
    const int HDRLEN= 0;
	const int STARTADDR= 16393;	// 0x4009

    ProgAddr = 16509;

    // Get the [D_FILE] system variable (PEEK 16396+256*PEEK 16397)
	pos= 16396 -STARTADDR +HDRLEN;
    VarsAddr= Get2(&mem[pos]);

    // seek to PROG area
	flen= VarsAddr -STARTADDR +HDRLEN;
	pos= ProgAddr -STARTADDR +HDRLEN;
    inFirstLineREM = (mem[pos +4] == REM_code);
    while (pos < flen) {	
        LineNum = 256*mem[pos] +mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = mem[pos +2] +256*mem[pos +3];
		if (LineLen > flen -pos -4) LineLen= flen -pos -4;

		memcpy(LineData, mem +pos +4 ,LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        printf("%4d", LineNum);
        DeTokenizeP(LineLen);
        inFirstLineREM = 0;
			
		pos= pos +2 +2 +LineLen; 	       
    }
	printf("\n");
	free(mem);
	return(0);

}

int P81PROC()
{
    const int HDRLEN= zlen(mem);
	const int STARTADDR= 16393;	// 0x4009

    ProgAddr = 16509;

    // Get the [D_FILE] system variable (PEEK 16396+256*PEEK 16397)
	pos= 16396 -STARTADDR +HDRLEN;
    VarsAddr= Get2(&mem[pos]);
	
    // seek to PROG area
	flen= VarsAddr -STARTADDR +HDRLEN;
	pos= ProgAddr -STARTADDR +HDRLEN;
    inFirstLineREM = (mem[pos +4] == REM_code);	
    while (pos < flen) {	
        LineNum = 256*mem[pos] +mem[pos +1];   
        if (LineNum > 16384) break;   //se salta la zona de vars tras programa

		LineLen = mem[pos +2] +256*mem[pos +3];
		if (LineLen > flen -pos -4) LineLen= flen -pos -4;

		memcpy(LineData, mem +pos +4 ,LineLen);
        LineData[LineLen]= 0; //Terminate the line data

        printf("%4d", LineNum);
        DeTokenizeP(LineLen);
        inFirstLineREM = 0;
			
		pos= pos +2 +2 +LineLen; 	       
    }
	printf("\n");
	free(mem);
	return(0);

}

int T81PROC()
{
    int hdrlen;
	int STARTADDR= 16393;	// 0x4009

    ProgAddr = 16509;
	
    pos= 4;	   
	j= pos;

    int ProgAddrFpos;
	int inDFILEpos;
    while (pos < flen){
          len= GetT81L(&mem[pos +0x20]);	      //Data Block length
		  //printf("pos=0x%X mem[0x%X]=0x%X len=%d\n\n", pos, pos+0x20, mem[pos+0x20], len);
		  hdrlen= 48 + zlen(mem+48 +pos);
          // Get the [D_FILE] system variable (PEEK 16396+256*PEEK 16397)
		  inDFILEpos= mem[16396 -STARTADDR +hdrlen + pos] + 256*mem[16396 +1 -STARTADDR +hdrlen + pos] -STARTADDR +hdrlen +pos;
          //printf("inDFILEpos=0x%X\n", inDFILEpos);

          if (ProgAddr -STARTADDR + hdrlen + j < inDFILEpos)
		    inFirstLineREM = (mem[ProgAddr -STARTADDR + hdrlen + j +4] == REM_code);
			
		  while (ProgAddr -STARTADDR + hdrlen + j < inDFILEpos) {
            ProgAddrFpos= ProgAddr -STARTADDR + hdrlen + j;
            //printf("ProgAArFpos=0x%X\n",ProgAddrFpos);		

			LineNum = 256*mem[ProgAddrFpos] +mem[ProgAddrFpos +1];
			if (LineNum > 16384) break;
			
			LineLen = mem[ProgAddrFpos +2] +256*mem[ProgAddrFpos +3];
            //if (LineLen > ProgLen -5) LineLen= ProgLen -5;	// flag(1) +LineNum(2) +LineLen(2)
	   
			memcpy(LineData, mem + ProgAddrFpos +4 ,LineLen);
	        LineData[LineLen]= 0; // Terminate the line data
	
            printf("%4d", LineNum);
            DeTokenizeP(LineLen);
            inFirstLineREM = 0;
			j= j +2 +2 +LineLen;
				
          }
		  printf("\n");
		  pos = pos +0x30 +len;	  // blklen before name (0x30) + len ¿+pause (0x30)?
          if (!strcmp(mem +pos,"<Silence>")) pos= pos +0x30;
		  j= pos;
    }
	free(mem);
}

