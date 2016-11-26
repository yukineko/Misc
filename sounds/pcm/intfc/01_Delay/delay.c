#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/wavheader.h"

int main(int argc, char* argv[]){
    char outname[255];
    int ret;

    if(argc < 2){
        fprintf(stderr, "Usage : ./delay [filename]\n");
        exit(EXIT_FAILURE);
    }

    memset(outname, 0, 255);
    strcpy(outname, argv[1]);    
    strcat(outname, "_out.wav");

    wavheader header, newheader;
    
    ret = readWavHeader(argv[1], &header);
    if(ret != 0){
        fprintf(stderr, "input file error\n");
        exit(-1);
    }
    dumpWavHeader(&header);

    size_t length = header.datasize + (8000 * sizeof(short));
    char* outbuffs = (char*)malloc(length);
    if(outbuffs == 0){
        fprintf(stderr, "malloc failed");
        exit(-1);
    }
    char buffs[8000];
    memset(outbuffs, 0, length);

    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 44L, SEEK_SET);
    size_t max_length =  header.datasize;
    for(size_t i = 0;i < max_length;i++){
        memset(buffs, 0, 8000);
        int readbytes = fread(buffs,  sizeof(char), 8000, fp);
        memcpy(outbuffs + (8000 * (i + 1)), buffs, readbytes);        
        if(readbytes < 8000){
            break;
        } 
    }
    
    newheader = header;
    newheader.datasize = newheader.datasize + 8000 * sizeof(short);
    newheader.filesize = newheader.filesize + 8000 * sizeof(short);

    dumpWavHeader(&newheader);

    addWavHeader(outname, &newheader);
    FILE* out = fopen(outname, "a+b");
    fwrite(outbuffs, sizeof(char), length, out);
    fclose(out);
    
    printf("%d\n", newheader.datasize);
    return 0;
}
