#include "sample-common.h"
#include <stdio.h>

__global__ void printMatrixIndex(int* mat, int nx, int ny)
{
    int ix = blockIdx.x * blockDim.x + threadIdx.x;
    int iy = blockIdx.y * blockDim.y + threadIdx.y;

    unsigned int idx = iy * nx + ix;
    printf("blockIdx(%d, %d), blockDim(%d, %d), threadIdx(%d, %d), idx = %d, val = %d\n",
           blockIdx.x, blockIdx.y,
           blockDim.x, blockDim.y,
           threadIdx.x, threadIdx.y,
           idx,
           mat[idx]
           );
    
}

int main(int argc, char* argv[]){
    printf("%s Starting...\n", argv[0]);
    int dev = 0;
    cudaDeviceInit(dev);

    int nx = 8;
    int ny = 6;
    int nxy = nx * ny;

    int nBytes = nxy * sizeof(int);
    int *h_A;
    h_A = (int*)malloc(nBytes);

    initDataSeq(h_A, nxy);

    printMatrixInt(h_A, nx, ny);

    int* d_Mat;
    CHECK(cudaMalloc((void**)&d_Mat, nBytes));
    CHECK(cudaMemcpy(d_Mat, h_A, nBytes, cudaMemcpyHostToDevice));

    dim3 block(4,2);
    dim3 grid((nx + block.x - 1) / block.x, (ny + block.y - 1) / block.y);
    printMatrixIndex<<<grid, block>>>(d_Mat, nx, ny);

    CHECK(cudaDeviceSynchronize());
    CHECK(cudaFree(d_Mat));
    free(h_A);

    CHECK(cudaDeviceReset());
    return 0;
}