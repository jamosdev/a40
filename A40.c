#include <library/StrStuff/A40.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <core/Macros/Endian.h>

/** we can't use 0xf0cc as that is inside valid input range, AND WE CAN'T EVEN USE 0xFACC as
even though that is outside the valid input range, it is actually still inside the valid
OUTPUT range so the round-trip gets messed up (output ranges up to 0xfbfa btw).*/
#define FUCK_SPELT_OUT_IN_HEX 0xFECC
uint16_t encoding_64k_tables[64000+1];
uint16_t decoding_64k_tables[65536];
static char tables_initialized=0;
static void ensure_64k_tables(){
	size_t i;
	uint16_t counter;
	uint16_t found;
	if(tables_initialized==2||tables_initialized==1) return;
	tables_initialized=1;
	/*--build forwards table--*/
	for(i=0,counter=1;i<64000+1;++i){
		encoding_64k_tables[i]=counter;
		do {
			++counter;
		} while((counter&0xFF00)==0||(counter&0x00FF)==0);
	}
	/*--build backwards table--*/
	found=0;
	for(i=0;i<65536;++i){ /*it is okay, it is size_t not uint16_t, relax*/
		if(found>64000) {
			decoding_64k_tables[i]=FUCK_SPELT_OUT_IN_HEX;
		} else if(encoding_64k_tables[found]==i) {
			decoding_64k_tables[i]=found;
			++found;
		} else {
			decoding_64k_tables[i]=FUCK_SPELT_OUT_IN_HEX;
		}
	}
	tables_initialized=2;
}
uint16_t encode_64k_to_64kb_no_nulls(uint16_t rawinput){
	while(tables_initialized!=2) ensure_64k_tables();
	if(rawinput>64000) return FUCK_SPELT_OUT_IN_HEX;
	return encoding_64k_tables[rawinput];
}
uint16_t decode_64k_from_64kb_no_nulls(uint16_t encodedinput){
	while(tables_initialized!=2) ensure_64k_tables();
	return decoding_64k_tables[encodedinput];
}





unsigned short downbase(char c){
	if(c >= '0' && c<='9') return 0 + (c - '0');
	if(c >= 'A' && c<='Z') return 10 + (c - 'A');
	if(c == '.') return 36;
	if(c == ' ') return 39;
	return 39;
}
char upbase(unsigned short us){
	if(us < 10) return '0'+us;
	if(us < 36) return 'A'-10+us;
	if(us==36) return '.';
	if(us==39) return ' ';
	return ' ';
}
void a40cp_enc(unsigned short *cp, const char *str3, unsigned oob){
	char c1=' ',c2=' ',c3=' ';
	unsigned short us1,us2,us3,ret;
    
    if(!cp) return;
    if(oob>0){
        if(oob < 1279) {
            *cp = 257+64000+oob;
            return;
        }
        else if(oob < 1536){
            *cp = oob - 1279;
            return;
        }
        else return;
    }
    
	if(str3 && *str3) { 
		c1=*str3;
		++str3;
		if(*str3) {
			c2=*str3;
			++str3;
			if(*str3) {
				c3=*str3;
			}
		}
	}
	
	if(c1 >= 'a' && c1 <= 'z') c1 = 'A'+(c1-'a');
	else if(c1!='.'&&!isalnum(c1))c1=' ';
	if(c2 >= 'a' && c2 <= 'z') c2 = 'A'+(c2-'a');
	else if(c1!='.'&&!isalnum(c2))c2=' ';
	if(c3 >= 'a' && c3 <= 'z') c3 = 'A'+(c3-'a');
	else if(c1!='.'&&!isalnum(c3))c3=' ';
	
	us1=downbase(c1);
	us2=downbase(c2);
	us3=downbase(c3);
	
	ret = us1;
	ret *= 40;
	ret += us2;
	ret *= 40;
	ret += us3;
    
    /*--apply no 0x00?? / 0x??00 offset--*/
    ret += 257;
    
    ret = BIG16(ret);
	*cp = ret;
	return;
}

int a40cp_isoob(uint16_t inputcp) {
    if(inputcp < 257) return 1;
    if(inputcp > 257 + 64000) return 1;
    return 0;
}
unsigned a40cp_getoob(uint16_t in){
     if(in < 257) {
        printf("oob mode big engage\n");
        return 1279 + in;
    }
    if(in > (40*40*40)+257) {
        printf("oob mode small engage\n");
        return in - 64257;/*max of 1279*/
    }
    return 0;
}
void a40cp_decode(uint16_t inputcp, char *outbuf, unsigned *outoob){
    printf("recieved input cp %h %04hx\n",inputcp);
}

unsigned a40cp_dec(const unsigned short *cp, char *str3){
	unsigned short in = *cp;
	unsigned short us1,us2,us3;
	char c1,c2,c3;
    in=BIG16(in);
    
    if(in < 257) {
        printf("oob mode big engage\n");
        return 1279 + in;
    }
    if(in > (40*40*40)+257) {
        printf("oob mode small engage\n");
        return in - 64257;/*max of 1279*/
    }
    if(!str3) {
        printf("error %d\n",in);
        return 0;
    }
    
    /*--remove no 0x00?? / 0x??00 offset--*/
    in -= 257;
    
	us3 = in % 40;
	in /= 40;
	us2 = in % 40;
	in /= 40;
	us1 = in;//%40
	c1 = upbase(us1);
	c2 = upbase(us2);
	c3 = upbase(us3);
	str3[0]=c1;
	str3[1]=c2;
	str3[2]=c3;
	return 0;
}
void a40toStr(char*destbuffer,void*a40bytes,size_t numa40cps){
    unsigned short *a40=(unsigned short*)a40bytes;
    while(numa40cps > 0){
        a40cp_dec(a40,destbuffer);
        ++a40;
        destbuffer+=3;
        --numa40cps;
    }
    *destbuffer='\0';
}
void strToA40(void*a40bytes,const char *srcStr,size_t numA40Cps){
    unsigned short *a40 = (unsigned short*)a40bytes;
    size_t i;
    for(i=0;i<numA40Cps;++i){
        if(!srcStr){
            a40cp_enc(a40,"   ",0);
            ++a40;
        } else {
            a40cp_enc(a40,srcStr,0);
            ++a40;
            if(strlen(srcStr)>3) srcStr+=3;
            else srcStr=NULL;
        }
    }
}

uint16_t a40_inline3char(char a, char b, char c) {
	uint16_t ret;
	char bf[3 + 1];
	bf[3] = '\0';
	bf[2] = c;
	bf[1] = b;
	bf[0] = a;
	a40cp_enc(&ret, bf, 0);
	return ret;
}
uint16_t a40_inline3str(const char* s) {
	uint16_t ret;
	/*return a40_inline3char(s[0],
		s[0]!='\0'?s[1]:' ',
		s[0] != '\0' && s[1] != '\0' ? s[2] : ' ');
		*/
	a40cp_enc(&ret, s, 0);
	return ret;
}

uint16_t a40_inlineoob(unsigned oob_lt_1536){
    uint16_t ret;
    //printf("encode %d\n",oob_lt_1536);
    a40cp_enc(&ret, NULL, oob_lt_1536);
    return ret;
}


void a40cstr_generate_sourcecode_for(const char *variablename, 
    const char *input_string, char *outputbuf, size_t out_sz){

}
void a40cstr_decode_to_asciibuf(const char *a40_cstr_nullterm,
    char *outascii, size_t outsz){

}
size_t a40cstr_strlen(const char *a40_cstr_nullterm){
	size_t len;
	if(a40_cstr_nullterm==NULL) return 0;
	if(a40_cstr_nullterm[0]=='\0') return 0;
	/*--nonsensical but do it anyway. len=0 because 1byte is
	0characters as a valid a40 code is 2bytes not 1byte--*/
	if(a40_cstr_nullterm[1]=='\0') return 0;
	/*--will eventually exit even if given an invalid string--*/
	for(len=2;len;len+=2){
		if(a40_cstr_nullterm[len]=='\0') return len;
	}
	return 0;
}
char a40cstr_get(const char*a40_cstr_nullterm, size_t virtualIndex){
	uint16_t cp;
	char decbuf[3];
	size_t cp = virtualIndex / 3;
	size_t offset = virtualIndex - (cp * 3);
	size_t address = cp * 2;
	cp = (unsigned char*)a40_cstr_nullterm[address];
	cp <<= 8;
	cp |= (unsigned char*)a40_cstr_nullterm[address];

}


static uint16_t remap_down(unsigned char inchar,const char *symbols4){
	unsigned char sym1 = (unsigned char*)symbols4[0];
	unsigned char sym2 = (unsigned char*)symbols4[1];
	unsigned char sym3 = (unsigned char*)symbols4[2];
	unsigned char sym4 = (unsigned char*)symbols4[3];
	
	if(inchar >= '0' && inchar <= '9') return 0+(inchar-'0');
	if(inchar >= 'A' && inchar <= 'Z') return 0+10+(inchar-'A');
	if(inchar == sym1) return 0+10+26+0;
	if(inchar == sym2) return 0+10+26+1;
	if(inchar == sym3) return 0+10+26+2;
	if(inchar == sym4) return 0+10+26+3;
	return 0+10+26+0;
}
static unsigned char reduce(unsigned char inchar,const char *symbols4){
	unsigned char sym1 = (unsigned char*)symbols4[0];
	unsigned char sym2 = (unsigned char*)symbols4[1];
	unsigned char sym3 = (unsigned char*)symbols4[2];
	unsigned char sym4 = (unsigned char*)symbols4[3];

	if(inchar >= '0' && inchar <= '9') {
		return inchar;
	}
	if(inchar >= 'A' && inchar <= 'Z') {
		return inchar;
	}
	if(inchar >= 'a' && inchar <= 'z') {
		return toupper(inchar);
	}
	if(inchar == sym1 || inchar == sym2 || inchar == sym3 || inchar == sym4) return inchar;
	return sym1;
}
static unsigned char remap_up(uint16_t element,const char *symbols4){
	unsigned char sym1 = (unsigned char*)symbols4[0];
	unsigned char sym2 = (unsigned char*)symbols4[1];
	unsigned char sym3 = (unsigned char*)symbols4[2];
	unsigned char sym4 = (unsigned char*)symbols4[3];
	
	if(element >= 0 && element <= 9) return '0'+element;
	if(element >= 10 && element <= 10+25) return 'A'+(element-10);
	if(element == 10+26+0) return sym1;
	if(element == 10+26+1) return sym2;
	if(element == 10+26+2) return sym3;
	if(element == 10+26+3) return sym4;
	return sym1;
}
void a40_cp2ascii(uint16_t cp, char *buff_sz_3,const char *symbols4){
	uint16_t val;
	uint16_t a,b,c;

	val = decode_64k_from_64kb_no_nulls(cp);
	c = val % 40;
	val /= 40;
	b = val % 40;
	val /= 40;
	a = val;

	buff_sz_3[0]=remap_up(a,symbols4);
	buff_sz_3[1]=remap_up(b,symbols4);
	buff_sz_3[2]=remap_up(c,symbols4);
}
char * a40_cp2ascii_nt(uint16_t cp, char *buff_sz_4,const char *symbols4){
	a40_cp2ascii(cp,buff_sz_4,symbols4);
	buff_sz_4[3]='\0';
	
	return buff_sz_4;
}
uint16_t a40_ascii2cp(const char *eats3,const char *symbols4){
	uint16_t val;
	uint16_t a,b,c;

	a=remap_down(reduce((unsigned char*)eats3[0],symbols4),symbols4);
	b=remap_down(reduce((unsigned char*)eats3[1],symbols4),symbols4);
	c=remap_down(reduce((unsigned char*)eats3[2],symbols4),symbols4);
	val = a;
	val *= 40;
	val += b;
	val *= 40;
	val += c;	
	return encode_64k_to_64kb_no_nulls(val);
}
