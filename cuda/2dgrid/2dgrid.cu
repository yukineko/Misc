#include <stdio.h>
#include "sample-common.h"

void sumMatrixOnHost(float* a, float* b, float* host, int x, int y){
    printf("matrix\n");
    float* ia = a;
    float* ib = b;
    float* ic = host;
    for(int i = 0;i < x; i++){
        for(int j = 0; j < y; j++){
            ic[j] = ia[j] + ib[j];
        }
        ia += x;
        ib += x;
        ic += x;
    }
}

__global__ void sumMatrixOnGpu(float* a, float* b, float* gpu, int x, int y){
    unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
    unsigned int iy = threadIdx.y + blockIdx.y * blockDim.y;
    unsigned int idx = iy * ix + x;
    if(ix < x && iy < y){
        gpu[idx] = a[idx] + b[idx];
    }
}


__global__ void sumMatrixUSING1D(float* a, float* b, float* gpu, int x, int y){
    unsigned int ix = threadIdx.x + blockIdx.x * blockDim.x;
    if(ix < x){
        for(int iy = 0; iy < y; iy++){
            int idx = iy * ix + x;
            gpu[idx] = a[idx] + b[idx];
        }
    }
}
int main(int argc, char* argv[]){
    printf("%s starting...\n", argv[0]);

    int dev = 0;
    cudaDeviceProp prop;
    CHECK(cudaGetDeviceProperties(&prop, dev));
    printf("Using Device %d\n", dev);

    
    CHECK(cudaSetDevice(dev));
    int nx = 1 << 13;
    int ny = 1 << 13;
    int nxy = nx * ny;
    size_t nBytes = nxy * sizeof(float);

    float *h_A, *h_B, *hostRef, *gpuRef;
    h_A = (float*)malloc(nBytes);
    h_B = (float*)malloc(nBytes);
    hostRef = (float*)malloc(nBytes);
    gpuRef = (float*)malloc(nBytes);

    double iStart, iElasp;
    initialData(h_A, nxy);
    initialData(h_B, nxy);
    clearData(hostRef, nxy);
    clearData(gpuRef, nxy);
    
    iStart = cpuSecond();
    sumMatrixOnHost(h_A, h_B, hostRef, nx, ny);
    iElasp = cpuSecond() - iStart;
    printf("sumMatrixOnHost %d %d  time=%f\n", nx, ny, iElasp);


    int dimx = 32, dimy = 16;

    float *d_A, *d_B, *d_C;
    CHECK(cudaMalloc((void**)&d_A, nBytes));
    CHECK(cudaMalloc((void**)&d_B, nBytes));
    CHECK(cudaMalloc((void**)&d_C, nBytes));
    CHECK(cudaMemcpy(d_A, h_A, nBytes, cudaMemcpyHostToDevice));
    CHECK(cudaMemcpy(d_B, h_B, nBytes, cudaMemcpyHostToDevice));
    
    dim3 block(dimx, dimy);
    dim3 grid((nx + block.x - 1) / block.x, (ny + block.y - 1) / block.y);
    iStart = cpuSecond();    
    sumMatrixOnGpu<<<grid, block>>>(d_A, d_B, d_C, nx, ny);
    CHECK(cudaDeviceSynchronize());
    iElasp = cpuSecond() - iStart;    
    printf("sumMatrixOnGpu<<<(%d, %d)(%d, %d)>>> %d, %d  time=%f\n",
           grid.x, grid.y,
           block.x, block.y,
           nx, ny,
           iElasp);
              
    CHECK(cudaGetLastError());
    CHECK(cudaMemcpy(gpuRef, d_C, nBytes, cudaMemcpyDeviceToHost));
    int ret = checkResult(hostRef, gpuRef, nxy);
    if(ret != 0){
        fprintf(stderr, "check error!!\n");
        exit(-1);
    }
    
    iStart = cpuSecond();    
    dim3 block2(32, 1);
    dim3 grid2((nx + block.x - 1) / block.x, 1);
    sumMatrixUSING1D<<<grid2, block2>>>(d_A, d_B, d_C, nx, ny);
    CHECK(cudaDeviceSynchronize());
    iElasp = cpuSecond() - iStart;    
    printf("sumMatrixUSING 1D<<<(%d, %d)(%d, %d)>>> %d, %d  time=%f\n",
           grid2.x, grid2.y,
           block2.x, block2.y,
           nx, ny,
           iElasp);
    CHECK(cudaGetLastError());
    CHECK(cudaMemcpy(gpuRef, d_C, nBytes, cudaMemcpyDeviceToHost));
    ret = checkResult(hostRef, gpuRef, nxy);
    if(ret != 0){
        fprintf(stderr, "check error!!\n");
        exit(-1);
    }
    
    
    CHECK(cudaFree(d_A));
    CHECK(cudaFree(d_B));
    CHECK(cudaFree(d_C));
    free(h_A);
    free(h_B);
    free(hostRef);
    free(gpuRef);    
    return 0;
}

