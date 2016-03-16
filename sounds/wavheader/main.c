#include <stdlib.h>
#include <err.h>
#include "wavheader.h"
int main(int argc, char* argv[]){
    if(argc < 3){
	err(EXIT_FAILURE, "usage : wavheader filename\n");
    }
    
    add_wav_header(22000, 1, argv[1], argv[2]);

    return 0;
}
