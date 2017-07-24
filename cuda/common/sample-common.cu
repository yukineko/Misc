#include "sample-common.h"
#include <stdio.h>
#include <sys/time.h>

double cpuSecond(){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double sec = ts.tv_sec;
    double usec = (double)ts.tv_nsec / (double)(1000 * 1000 * 1000);
    //    printf("sec = %f\n", sec);
    //    printf("usec = %f\n", usec);
    return sec + usec;
}

void initialData(float* data, int size){
    time_t t;
    srand((unsigned) time(&t));

    for(int i = 0; i < size; i++){
        data[i] = (float)(rand() & 0xff) / 10.0f;
    }
}

void initialDataInt(int* data, int size){
    time_t t;
    srand((unsigned) time(&t));

    for(int i = 0; i < size; i++){
        data[i] = (int)(rand() & 0xff);
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

int checkResult(float* A, float* B, size_t size){
    for(int i = 0;i < size; i++){
        if(A[i] != B[i]){
            return i;
        }
    }
    return 0;
}
