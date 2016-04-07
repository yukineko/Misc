#include "wavheader.h"
#include <stdio.h>   //
#include <stdlib.h>  // EXIT_FAILURE
#include <err.h> // err
#include <sys/types.h> // fstat
#include <sys/stat.h>  // fstat
#include <unistd.h>    // fstat
#include <stdint.h> // int32_t
// headerは http://home.a00.itscom.net/hatada/asp/wav.html
// を参考にしました

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

static long get_pcm_filesize(FILE* pcm_file_hd){
    // fseekとかftellとか使用しない
    // https://www.jpcert.or.jp/sc-rules/c-fio19-c.html
     struct stat fstat_result;
     if(fstat(fileno(pcm_file_hd), &fstat_result) == 0){
	 return fstat_result.st_size;
     }
     return -1;
}



int add_wav_header(unsigned int bitrate, unsigned int channels, char* pcm_filename, char* output_file){
    int result;
    long pcmsize = -2;
    unsigned int buffer[1024];

    wavheader wavh = {"RIFF", 0, "WAVE", "fmt ", 16, 1, 1, 44100, 44100, 1, 16, "data", 0};

    FILE* pcmh = fopen(pcm_filename, "rb");
    if(pcmh == NULL){
	err(EXIT_FAILURE, "failed to open %s\n", pcm_filename);
    }

    pcmsize = get_pcm_filesize(pcmh);
    if(pcmsize < 0){
	err(EXIT_FAILURE, "file size error %ld / %s\n", pcmsize, pcm_filename);
    }

    int buffer_coeff = 16 / 8;
    wavh.channels = channels;
    wavh.sampling_rate = bitrate;
    wavh.buffersize = bitrate * channels * buffer_coeff; 
    wavh.filesize = 44 - 8 + pcmsize;
    wavh.datasize = pcmsize;

    FILE* outh = fopen(output_file, "wb");
    if(outh == NULL){
	err(EXIT_FAILURE, "failed to open %s\n", output_file);
    }

    fwrite(&wavh, sizeof(wavh), 1, outh);
    
    while((result = fread(buffer, sizeof(int), 1024, pcmh)) != 0){
	fwrite(&buffer, sizeof(int), result, outh);
    }
    fclose(pcmh);
    fclose(outh);
    return 0;
}
