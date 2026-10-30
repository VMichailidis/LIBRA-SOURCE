#ifndef _CUDANAIVE_HPP
#define _CUDANAIVE_HPP
#include "../../util/Unrolled.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#define THREAD_LIMIT 64

template <typename T, unsigned char LOG_N>
__global__ void kernel_swap_inplace(T *srcDst) {
    unsigned long index = (unsigned long)blockIdx.x * blockDim.x + threadIdx.x;
    if (index >= (1ul << LOG_N))
        return;
    unsigned long reversed = __brev((unsigned)index) >> (32 - LOG_N);
    if (reversed < index) {
        T tmp = srcDst[index];
        srcDst[index] = srcDst[reversed];
        srcDst[reversed] = tmp;
    }
}
template <typename T, unsigned char NUM_BITS> void naiveGpuApply(T *input_array) {
    // here the data is on GPU
    // this part will recurse later
    const unsigned long width = 1ul << NUM_BITS;

    int blockSize;
    int gridSize;

    if (width < THREAD_LIMIT) {
        blockSize = width;
        gridSize = 1;
    } else {
        gridSize = (width + THREAD_LIMIT - 1) / THREAD_LIMIT;
        blockSize = THREAD_LIMIT;
    }

    kernel_swap_inplace<T, NUM_BITS><<<gridSize, blockSize>>>(input_array);
}

template <typename T, unsigned char NUM_BITS> class CUDANaive {
  public:
    //  __attribute__((always_inline))
    static void apply(T *__restrict input_on_host) {
        // move the data to gpu
        const unsigned long N = 1ul << NUM_BITS;

        T *input_on_device = 0;
        cudaError_t cudaStatus;

        // Choose which GPU to run on, change this on a multi-GPU
        // system.
        cudaStatus = cudaSetDevice(0);
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaSetDevice failed!  Do you have a "
                            "CUDA-capable GPU installed?");
            goto Error;
        }

        // Allocate GPU buffers for three vectors (two input, one
        // output)    .
        cudaStatus = cudaMalloc((void **)&input_on_device, N * sizeof(T));
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaMalloc failed!");
            goto Error;
        }

// Copy input vectors from host memory to GPU buffers.
#ifndef TESTING
        cudaStatus = cudaMemcpy(input_on_device, input_on_host, N * sizeof(T),
                                cudaMemcpyHostToDevice);
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaMemcpy failed!");
            goto Error;
        }
#endif
        naiveGpuApply<T, NUM_BITS>(input_on_device);

// Copy output vector from GPU buffer to host memory.
#ifndef TESTING
        cudaStatus = cudaMemcpy(input_on_host, input_on_device, N * sizeof(T),
                                cudaMemcpyDeviceToHost);
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaMemcpy failed!");
            goto Error;
        }
#endif

    // is this evil
    Error:
        cudaFree(input_on_device);
        return;
    }
};
#endif
