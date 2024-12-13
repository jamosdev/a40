/**@file A40.h
@brief A40 (3in2) text encoding system. For saving space.
*/
#ifndef library_StrStuff_A40_h
#define library_StrStuff_A40_h
#include <stddef.h>
#include <stdint.h>

uint16_t encode_64k_to_64kb_no_nulls(uint16_t rawinput);
uint16_t decode_64k_from_64kb_no_nulls(uint16_t encodedinput);





/** as an alternative to SMAZ style text string compression */
void a40cstr_generate_sourcecode_for(const char *variablename, 
    const char *input_string, char *outputbuf, size_t out_sz);
/** null terminated cstring buffer at once decoder */
void a40cstr_decode_to_asciibuf(const char *a40_cstr_nullterm,
    char *outascii, size_t outsz);
/** null terminated cstring encoded strlen */
size_t a40cstr_strlen(const char *a40_cstr_nullterm);
/** null terminated cstring encoded virtual iterator */
char a40cstr_get(const char*a40_cstr_nullterm, size_t virtualIndex);

/** null terminates buff_sz_4 for you so that you can use the
char* return value (which is buff_sz_4) straight away */
char * a40_cp2ascii_nt(uint16_t cp, char *buff_sz_4,const char *symbols4);
void a40_cp2ascii(uint16_t cp, char *buff_sz_3,const char *symbols4);
uint16_t a40_ascii2cp(const char *eats3,const char *symbols4);

void a40toStr(char*destbuffer,void*a40bytes,size_t numa40cps);
void strToA40(void*a40bytes,const char *srcStr,size_t numA40Cps);

/**
@param oob
 leave as 0 unless you want to encode an "oob" number
 instead of encoding str3 (which can be NULL then).
*/
void a40cp_enc(unsigned short *cp, const char *str3, unsigned oob);
/**
@return
normally returns 0 but if the cp encoded an "oob" number,
then it will return that instead, and str3 will not be
touched
*/
unsigned a40cp_dec(const unsigned short *cp, char *str3);
uint16_t a40_inline3char(char a, char b, char c);
uint16_t a40_inline3str(const char *s);
uint16_t a40_inlineoob(unsigned oob_lt_1536);
void a40cp_decode(uint16_t inputcp, char *outbuf, unsigned *outoob);
int a40cp_isoob(uint16_t inputcp);
unsigned a40cp_getoob(uint16_t in);
#endif//library_StrStuff_A40_h
