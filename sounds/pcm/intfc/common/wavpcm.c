#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavpcm.h"

#define DATA_SIZE 8000
void normalizePcmDataS16(signed short *buffs, double* normalized, size_t len){
    size_t p;
    for(p = 0; p < len; p++){
        normalized[p] = buffs[p] / DOUBLE_MAX;
    }
}

void normalized2PcmDataS16(double* normalized, signed short *buffs, size_t len){
    size_t p;
    for(p = 0; p < len; p++){
        buffs[p] = (short)(normalized[p] * DOUBLE_MAX);
    }
}

void normalizePcmDataS16fp(FILE* fp,
                           double* input_data){
    signed short buffs[DATA_SIZE];
    memset(buffs, 0, DATA_SIZE / sizeof(short));
    int i = 0;
    while(1){
        size_t readcount = fread(buffs, sizeof(short), DATA_SIZE, fp);
        normalizePcmDataS16(buffs, input_data + (i * DATA_SIZE), readcount);
        if(readcount < DATA_SIZE){
            break;
        }
        i++;
    }
    
}
