#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/wavheader.h"
#include "../common/wavpcm.h"
#include "../common/util.h"
#define BUFF_SIZE 8000

int main(int argc, char* argv[]){
    char outname[255];
    int ret;

    usage(argv[0], argc, argv[1], outname);
    
    wavheader header, newheader;
    ret = readWavHeader(argv[1], &header);
    if(ret != 0){
        fprintf(stderr, "input file error\n");
        exit(-1);
    }
    dumpWavHeader(&header);

    newheader = header;

    size_t frames = header.datasize / (sizeof(short));
    double* input_data = (double*)malloc(frames * sizeof(double));
    
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 44L, SEEK_SET);
    normalizePcmDataS16fp(fp, input_data);
    
    free(input_data);
    return 0;
}
