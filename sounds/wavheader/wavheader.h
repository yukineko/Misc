#ifndef __WAVHEADER_H__
#define __WAVHEADER_H__

int add_wav_header(unsigned int bitrate, 
		   unsigned int channels, 
		   char* pcm_filaname, 
		   char* output_file);
#endif
