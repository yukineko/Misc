#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

void usage(char* command, int argc, char* filename, char* outname){
    if(argc < 2){
        fprintf(stderr, "Usage : ./%s [filename]\n", command);
        exit(EXIT_FAILURE);
    }

    memset(outname, 0, 255);
    strcpy(outname, filename);    
    strcat(outname, "_out.wav");
    
}
