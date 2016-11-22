#include <stdio.h>
#include <stdlib.h>
#include "../common/wavheader.h"
#include "../common/wavpcm.h"

int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(stderr, "Usage ...");
        exit(-1);
    }

    wavheader header, newheader;
    readWavHeader(argv[1], &header);
    dumpWavHeader(&header);

    newheader = header;
    dumpWavHeader(&newheader);
    addWavHeader(argv[2], &newheader);
    
    signed short* buffs = malloc(header.sampling_rate * header.blocksize);
    double* nbuffs = malloc(header.sampling_rate * sizeof(double));

    
    FILE* f1 = fopen(argv[1], "rb");
    if(f1 == NULL){
        fprintf(stderr, "read file failed to open %s\n", argv[1]);
        exit(-1);
    }
    FILE* f2 = fopen(argv[2], "ab+");
    if(f2 == NULL){
        fprintf(stderr, "write file failed to open %s\n", argv[2]);
        exit(-1);
    }

    free(buffs);
    free(nbuffs);
    return 0;
}
