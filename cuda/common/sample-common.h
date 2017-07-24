#ifndef _CUDA_SAMPLE_COMMON_H_

#define _CUDA_SAMPLE_COMMON_H_
#define CHECK(call)                                                               \
{                                                                                 \
    const cudaError_t error = call;                                               \
    if(error != cudaSuccess){                                                     \
        printf("Error : %s %d, ", __FILE__, __LINE__);                            \
        printf("error code %d: reason %s \n", error, cudaGetErrorString(error));  \
        exit(1);                                                                  \
    }                                                                             \
}                                                                                 \

double cpuSecond();
void initialData(float* data, int size);
void initialDataInt(int* data, int size);
void clearData(float* data, int size);
void printMatrixInt(int* A, const int x, const int y);
void initAtNum(int* A, size_t size, int val);
void initDataSeq(int* A, size_t size);
int checkResult(float* A, float* B, size_t size);
void cudaDeviceInit(int dev);
#endif
