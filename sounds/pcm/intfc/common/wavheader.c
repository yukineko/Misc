#include "wavheader.h"
#include <stdio.h>   //
#include <stdlib.h>  // EXIT_FAILURE

int readWavHeader(const char* path, wavheader* header){
    FILE *f1;
    f1 = fopen(path, "rb");
    if(f1 == NULL){
        fprintf(stderr, "can't ooen file %s", path);
        exit(-2);
    }
    int ret = fread(header, 44L, 1, f1);
    if(ret == 0){
        fprintf(stderr, "no data wavheader\n");
        exit(-3);
    }
    
    fclose(f1);
    return 0;
}

void addWavHeader(const char* path, wavheader* header){
    FILE* f = fopen(path, "wb");
    if(f == NULL){
        fprintf(stderr, "can't open file %s", path);
        exit(-2);
    }
    fseek(f, 0, SEEK_SET);
    int ret = fwrite(header, sizeof(wavheader), 1, f);
    if(ret == 0){
        fprintf(stderr, "no data written\n");
        exit(-3);
    }
    fclose(f);
}

void dumpWavHeader(wavheader* header){
    printf("****************************\n");
    printf("address   \t%lx\t\n\n", (long int)header);
    printf("RIFF      \t%s\n", header->riff);
    printf("Size      \t%d\n", header->filesize);
    printf("Type      \t%s\n", header->wave);
    printf("ID        \t%s\n", header->fmt);
    printf("FormatSize\t%d\n", header->fmtsize);
    printf("Format    \t%d\n", header->format);
    printf("Channel   \t%d\n", header->channels);
    printf("Rate      \t%d\n", header->sampling_rate);
    printf("buffersize\t%d\n", header->buffersize);
    printf("blocksize \t%d(bytes)\n", header->blocksize);
    printf("bitsample \t%d\n", header->bitper_sample);
    printf("DATA      \t%s\n", header->data);
    printf("datasize  \t%d\n\n\n", header->datasize);
}
