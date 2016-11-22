#include <stdint.h> // int32_t

#ifndef __WAVHEADER_H__
#define __WAVHEADER_H__

typedef struct {
    char riff[4];
    int32_t filesize;
    char wave[4];
    char fmt[4];
    int32_t fmtsize;
    int16_t format;
    int16_t channels;
    int32_t sampling_rate;
    int32_t buffersize;
    int16_t blocksize;
    int16_t bitper_sample;
    char data[4];
    int32_t datasize;
} wavheader;

void addWavHeader(const char* path, wavheader* header);
int readWavHeader(const char* path, wavheader* header);
void dumpWavHeader(wavheader* header);

#endif
