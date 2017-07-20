#include <cuda_runtime.h>
#include <stdio.h>
#include <sys/time.h>

#include "sample-common.h"
#define LENGTH 1024


void sumArrayOnHost(float* A, float* B, float* C, int size){
    for(int idx = 0; idx < size; idx++){
        C[idx] = A[idx] + B[idx];
    }
}

__global__ void sumArrayOnGpu(float* A, float* B, float* C, int size){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    // printf("blockIdx.x = %d, blockDim.x = %d, threadIdx.x = %d, value = %d\n",
    //        blockIdx.x,
    //        blockDim.x,
    //        threadIdx.x,
    //        i
    //        );
    if(i < size){
       C[i] = A[i] + B[i];
    }
}

void testAtHost(float* h_A, float* h_B, float* hostRef, float* gpuRef, int size){
    double iStart, iElaps;    
    iStart = cpuSecond();
    sumArrayOnHost(h_A, h_B, hostRef, size);
    iElaps = cpuSecond() - iStart;    
    printf("sumArrayOnHost Time elapsed %f sec\n", iElaps);
}

void testAtGpu(float* h_A, float* h_B, float* gpuRef, int size){
    float *d_A, *d_B, *d_C;
    int nBytes = size * sizeof(float);
    CHECK(cudaMalloc((float**)&d_A, nBytes));
    CHECK(cudaMalloc((float**)&d_B, nBytes));
    CHECK(cudaMalloc((float**)&d_C, nBytes));
    
    CHECK(cudaMemcpy(d_A, h_A, nBytes, cudaMemcpyHostToDevice));
    CHECK(cudaMemcpy(d_B, h_B, nBytes, cudaMemcpyHostToDevice));
    CHECK(cudaMemcpy(d_C, gpuRef, nBytes, cudaMemcpyHostToDevice));
    
    double iStart, iElasp;
    
    dim3 block(LENGTH);
    dim3 grid((size + block.x -1) / block.x);
    iStart = cpuSecond();    
    sumArrayOnGpu<<<grid, block>>>(d_A, d_B, d_C, size);

    CHECK(cudaDeviceSynchronize());
    CHECK(cudaGetLastError());
    CHECK(cudaMemcpy(gpuRef, d_C, nBytes, cudaMemcpyDeviceToHost));
    iElasp = cpuSecond() - iStart;
    printf("<<<grid %d, block %d>>> %f sec\n", grid.x, block.x, iElasp);

    CHECK(cudaFree(d_A));
    CHECK(cudaFree(d_B));
    CHECK(cudaFree(d_C));
}
    

int checkResult(float* A, float* B, int size){
    for(int i = 0;i < size; i++){
        if(A[i] != B[i]){
            return i;
        }
    }
    return 0;
}


int main(int argc, char* argv[]){
    printf("%s starting ..\n", argv[0]);

    int dev = 0;
    cudaDeviceProp deviceProp;

    CHECK(cudaGetDeviceProperties(&deviceProp, dev));
    printf("Using Device %d: %s,\n", dev, deviceProp.name);
    CHECK(cudaSetDevice(dev));
    int nElem = 1 << 24;
    
    printf("Vector size %d\n", nElem);
    float *h_A, *h_B, *gpuRef, *hostRef;
    //    init(h_A, h_B, gpuRef, hostRef, nElem);
    size_t nBytes = nElem * sizeof(float);
    h_A = (float*)malloc(nBytes);
    h_B = (float*)malloc(nBytes);
    hostRef = (float*)malloc(nBytes);
    gpuRef = (float*)malloc(nBytes);

    initialData(h_A, nElem);
    initialData(h_B, nElem);
    clearData(hostRef, nElem);
    clearData(gpuRef, nElem);
    
    testAtGpu(h_A, h_B, gpuRef, nElem);
    testAtHost(h_A, h_B, hostRef, gpuRef, nElem);    
    int nRet = checkResult(hostRef, gpuRef, nElem);
    if( nRet != 0 ){
        printf("check result error %d\n", nRet);
    }
    
    free(h_A);
    free(h_B);
    free(hostRef);
    free(gpuRef);
    
    return 0;
}
