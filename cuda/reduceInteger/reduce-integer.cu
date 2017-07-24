#include "sample-common.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int recursiveReduce(int* data, size_t size){
    if(size == 1){
        return data[0];
    }

    size_t stride = size / 2;

    for(int i = 0; i < stride; i++){
        data[i] += data[i + stride];
    }
    return recursiveReduce(data, stride);
}

int testCpuRecursiveReduce(int* tmp, size_t size){
    double iStart, iElasp;
    
    iStart = cpuSecond();
    int cpu_sum = recursiveReduce(tmp, size);
    iElasp = cpuSecond() - iStart;
    printf("cpu reduce elapsed %f sec cpu_sum %d\n", iElasp, cpu_sum);
    return cpu_sum;
    
}

__global__ void reduceNeiborbored(int *g_idata, int *g_odata, unsigned int size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int* idata = g_idata + blockIdx.x * blockDim.x;
    if(idx >= size){
        return;
    }

    for(int stride = 1; stride < blockDim.x; stride *= 2){
        if((tid % (2 * stride)) == 0){
            idata[tid] += idata[tid + stride];
        }
        __syncthreads();        
    }

    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
}

int testGpuReduceNeiborbored(int* data, size_t size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceNeiborbored<<<grid, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x; i++){
        gpu_sum += result[i];
    }
    // free result
    free(result);
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Neiborbored elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x, block.x);
    return 0;
}

__global__ void reduceNeiborless(int* g_idata, int* g_odata, size_t size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int *idata = g_idata + blockIdx.x * blockDim.x;
    if(idx >= size){
        return;
    }

    for(int stride = 1; stride < blockDim.x; stride *= 2){
        int index = 2 * stride * tid;
        if(index < blockDim.x){
            idata[index] += idata[index + stride];
        }
        __syncthreads();
    }
    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
}


int testGpuReduceNeiborless(int* data, unsigned int size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceNeiborless<<<grid, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x; i++){
        gpu_sum += result[i];
    }
    // free result
    free(result);
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Neiborless elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x, block.x);
    return gpu_sum;
}


__global__ void reduceInterleave(int* g_idata, int* g_odata, size_t size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int *idata = g_idata + blockIdx.x * blockDim.x;
    if(idx >= size){
        return;
    }

    for(int stride = blockDim.x / 2; stride > 0; stride >>= 1){
        if(tid < stride){
            idata[tid] += idata[tid + stride];
        }
        __syncthreads();
    }
    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
}


int testGpuReduceInterleave(int* data, size_t size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceInterleave<<<grid, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x; i++){
        gpu_sum += result[i];
    }
    // free result
    free(result);
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Interleave elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x, block.x);
    return gpu_sum;    
}


__global__ void reduceUnrolling2(int* g_idata, int* g_odata, size_t size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x * 2 + threadIdx.x;
    int *idata = g_idata + blockIdx.x * blockDim.x * 2;
    if(idx + blockDim.x < size){
        g_idata[idx] += g_idata[idx + blockDim.x];
    }
    __syncthreads();

    for(int stride = blockDim.x / 2; stride > 0; stride >>= 1){
        if(tid < stride){
            idata[tid] += idata[tid + stride];
        }
        __syncthreads();
    }

    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
}

int testGpuReduceUnrolling2(int* data, size_t size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceUnrolling2<<<grid.x / 2, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes / 2, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x / 2; i++){
        gpu_sum += result[i];
    }
    // free result
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    free(result);
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Unroll 2 elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x / 2, block.x);
    return gpu_sum;    
}


__global__ void reduceUnrolling8(int* g_idata, int* g_odata, size_t size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x * 8 + threadIdx.x;
    int *idata = g_idata + blockIdx.x * blockDim.x * 8;
    if(idx + 7 * blockDim.x < size){
        int a0 = g_idata[idx];
        int a1 = g_idata[idx + blockDim.x];
        int a2 = g_idata[idx + 2 * blockDim.x];
        int a3 = g_idata[idx + 3 * blockDim.x];
        int b0 = g_idata[idx + 4 * blockDim.x];
        int b1 = g_idata[idx + 5 * blockDim.x];
        int b2 = g_idata[idx + 6 * blockDim.x];
        int b3 = g_idata[idx + 7 * blockDim.x];
        g_idata[idx] = a0 + a1 + a2 + a3 + b0 + b1 + b2 + b3;
    }
    __syncthreads();

    for(int stride = blockDim.x / 2; stride > 32; stride >>= 1){
        if(tid < stride){
            idata[tid] += idata[tid + stride];
        }
        __syncthreads();
    }

    if(tid < 32){
        volatile int * vmem = idata;
        vmem[tid] += vmem[tid + 32];
        vmem[tid] += vmem[tid + 16];
        vmem[tid] += vmem[tid + 8];
        vmem[tid] += vmem[tid + 4];
        vmem[tid] += vmem[tid + 2];
        vmem[tid] += vmem[tid + 1];
    }

    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
}

int testGpuReduceUnrolling8(int* data, size_t size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceUnrolling8<<<grid.x / 8, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes / 8, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x / 8; i++){
        gpu_sum += result[i];
    }
    // free result
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    free(result);
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Unroll 8 elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x / 8, block.x);
    return gpu_sum;    
}

__global__ void reduceCompleteUnroll(int* g_idata, int* g_odata, size_t size){
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x * 8 + threadIdx.x;
    int *idata = g_idata + blockIdx.x * blockDim.x * 8;
    if(idx + 7 * blockDim.x < size){
        int a0 = g_idata[idx];
        int a1 = g_idata[idx + blockDim.x];
        int a2 = g_idata[idx + 2 * blockDim.x];
        int a3 = g_idata[idx + 3 * blockDim.x];
        int b0 = g_idata[idx + 4 * blockDim.x];
        int b1 = g_idata[idx + 5 * blockDim.x];
        int b2 = g_idata[idx + 6 * blockDim.x];
        int b3 = g_idata[idx + 7 * blockDim.x];
        g_idata[idx] = a0 + a1 + a2 + a3 + b0 + b1 + b2 + b3;
    }
    __syncthreads();
    if(blockDim.x >= 1024 && tid < 512){
        idata[tid] += idata[tid + 512];
        __syncthreads();
    }
    if(blockDim.x >= 512 && tid < 256){
        idata[tid] += idata[tid + 256];
        __syncthreads();
    }
    if(blockDim.x >= 256 && tid < 128){
        idata[tid] += idata[tid + 128];
        __syncthreads();
    }

    if(blockDim.x >= 128 && tid < 64){
        idata[tid] += idata[tid + 64];
        __syncthreads();
    }

    if(tid < 32){
        volatile int * vmem = idata;
        vmem[tid] += vmem[tid + 32];
        vmem[tid] += vmem[tid + 16];
        vmem[tid] += vmem[tid + 8];
        vmem[tid] += vmem[tid + 4];
        vmem[tid] += vmem[tid + 2];
        vmem[tid] += vmem[tid + 1];
    }

    if(tid == 0){
        g_odata[blockIdx.x] = idata[0];
    }
   
}
int testReduceCompleteUnroll(int* data, size_t size){
    double iStart, iElasp;

    int *d_idata, *d_odata;
    int blocksize = 512;

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);
    
    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    
    // malloc result
    int* result = (int*) malloc(gridbytes);
    printf("grid %d block %d\n", grid.x, block.x);
    
    CHECK(cudaMalloc((void**)&d_idata, bytes));
    CHECK(cudaMalloc((void**)&d_odata, gridbytes));
    CHECK(cudaMemcpy(d_idata, data, bytes, cudaMemcpyHostToDevice));
    CHECK(cudaDeviceSynchronize());    
    iStart = cpuSecond();
    reduceUnrolling8<<<grid.x / 8, block>>>(d_idata, d_odata, size);
    CHECK(cudaDeviceSynchronize());
    CHECK(cudaMemcpy(result, d_odata, gridbytes / 8, cudaMemcpyDeviceToHost));
    int gpu_sum = 0;
    for(int i = 0; i < grid.x / 8; i++){
        gpu_sum += result[i];
    }
    // free result
    CHECK(cudaFree(d_idata));
    CHECK(cudaFree(d_odata));    
    free(result);
    
    iElasp = cpuSecond() - iStart;    
    printf("gpu Unroll Complete elapsed %f sec, gpu_sum = %d, <<< grid %d, block %d >>>\n", iElasp, gpu_sum, grid.x / 8, block.x);
    return gpu_sum;        
}


int main(int argc, char* argv[]){
    int dev = 0;
    printf("%s starting.\n", argv[0]);
    cudaDeviceProp prop;
    CHECK(cudaGetDeviceProperties(&prop, dev));

    printf("device %d, %s\n", dev, prop.name);
    CHECK(cudaSetDevice(dev));

    bool bResult = false;
    int size = 1 << 24;
    printf("with arrray size = %d\n", size);

    int blocksize = 512;
    if(argc > 1){
        blocksize = atoi(argv[1]);
    }

    dim3 block(blocksize, 1);
    dim3 grid((size + block.x - 1) / block.x, 1);

    size_t bytes = size * sizeof(int);
    size_t gridbytes = grid.x * sizeof(int);
    int *h_idata = (int *)malloc(bytes);
    int *h_odata = (int *)malloc(gridbytes);
    int *tmp = (int *)malloc(bytes);

    initialDataInt(h_idata, size);
    
    memcpy(tmp, h_idata, bytes);
    int result1 = testGpuReduceNeiborbored(tmp, size);
    
    memcpy(tmp, h_idata, bytes);        
    int result2 = testCpuRecursiveReduce(tmp, size);

    memcpy(tmp, h_idata, bytes);
    int result3 = testGpuReduceNeiborless(tmp, size);

    memcpy(tmp, h_idata, bytes);
    int result4 = testGpuReduceInterleave(tmp, size);

    memcpy(tmp, h_idata, bytes);
    int result5 = testGpuReduceUnrolling2(tmp, size);

    memcpy(tmp, h_idata, bytes);
    int result6 = testGpuReduceUnrolling8(tmp, size);

    memcpy(tmp, h_idata, bytes);    
    int result7 = testReduceCompleteUnroll(tmp, size);
    return 0;
}