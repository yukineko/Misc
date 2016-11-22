#ifndef _WAVPCM_H_
#define _WAVPCM_H_

#include <stddef.h>

#define DOUBLE_MAX 32768.0
void normalizePcmDataS16(signed short *buffs, double* normalized, size_t len);
void normalized2PcmDataS16(double* normalized, signed short *buffs, size_t len);
#endif

