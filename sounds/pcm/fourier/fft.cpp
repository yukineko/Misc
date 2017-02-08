#include "../common/wavpcm.h"

#define FFT_SIZE 512

static double Fr[FFT_SIZE + 1];
void fft(){
    int i;
    for(i = 0; i < FFT_SIZE; i++){
        
    }
}


int main(int argc, char* argv[]){
    short short_buffer[512];
    double double_buffer[512];
    if(argc < 2){
        fpritnf(stderr, "usage; fft [filename]\n");
        exit(-1);
    }
    FILE fh = fopen(argv[1], "rb");
    if(fh == nullptr){
        fprintf(stderr, "file open error\n");
        exit(-1);
    }

    while(1){
        int ret = fread(short_buffer, sizeof(short), 512, fh);
        if(ret == 0){
            break;
        }
        normalizePcmDataS16(short_buffer, double_buffer, ret);
        while(1){
            
        }
    }
    fclose(fh);
    return 0;
}
