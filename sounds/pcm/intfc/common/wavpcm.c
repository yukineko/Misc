#include "wavpcm.h"
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
