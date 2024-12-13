#include <stdio.h>
#include <library/StrStuff/A40.h>
int main(int argc, char **argv){
    size_t i;
    for(i=0;i<65536;++i){
        printf("%5u: \t(%s) encode(%u)->%u encoded output value ($%04x->$%04x) which decodes back into %5u\n",
            (unsigned)i,
            i>64000?"Invalide a40 cp":"valid a40 cp",
            (unsigned)i,
            (unsigned)encode_64k_to_64kb_no_nulls(i),
            (unsigned)i,
            (unsigned)encode_64k_to_64kb_no_nulls(i),
            (unsigned)decode_64k_from_64kb_no_nulls(encode_64k_to_64kb_no_nulls(i)));
    }
    return 0;
}
