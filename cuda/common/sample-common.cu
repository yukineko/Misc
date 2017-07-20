#include "sample-common.h"
#include <stdio.h>
#include <sys/time.h>

double cpuSecond(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6 );
}

void initialData(float* data, int size){
    time_t t;
    srand((unsigned) time(&t));

    for(int i = 0; i < size; i++){
        data[i] = (float)(rand() & 0xff) / 10.0f;
    }
}

void clearData(float* data, int size){
    for(int i = 0; i < size; i++){
        data[i] = 0;
    }    
}

void initAtNum(int* A, size_t size, int val)
{
    for(int i = 0;i < size; i++){
        A[i] = val;
    }
}

void initDataSeq(int* A, size_t size)
{
    for(int i = 0;i < size; i++){
        A[i] = i;
    }
}

void printMatrixInt(int* A, const int x, const int y){
    int *ic = A;
    printf("Matrix: (%d, %d)\n", x, y);
    for(int iy = 0; iy < y; iy++){
        for(int ix = 0; ix < x; ix++){
            printf("%3d, ", ic[ix]);
        }
        ic += x;
        printf("\n");
    }
    printf("\n");
}

void cudaDeviceInit(int dev){
    cudaDeviceProp prop;
    CHECK(cudaGetDeviceProperties(&prop, dev));
    printf("Using device %d, %s\n", dev, prop.name);
    CHECK(cudaSetDevice(dev));
}
