#ifndef _CULIBRA_SHUFFLE_HPP
#define _CULIBRA_SHUFFLE_HPP
#include "cuda_runtime.h"
#include <cstdio>
#include <cstdlib>

/* LOG_BLOCK_WIDTH should follow the relation 2*LBW < 18 -
 * log_2(sizeof(T)) so that each buffer fits inside an L1 cache
 */
#include <cuda_runtime_api.h>
#include <driver_types.h>
#include <stdio.h>
#include <vector>

#define THREAD_LIMIT 32
#ifndef gpuErrcheck
#define gpuErrcheck(ans)                                                                 \
    {                                                                                    \
        gpuAssert((ans), __FILE__, __LINE__);                                            \
    }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort = true) {
    if (code != cudaSuccess) {
        fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort)
            exit(code);
    }
}
#endif
template <const char W> __host__ __device__ int reverse(int x) {
    int reversed = 0;
    for (int j = 0; j < W; j++) // log2(N) = 3 bits needed to represent indices
        reversed = (reversed << 1) | (x >> j & 1);
    return reversed;
}

template <typename T, unsigned char LOG_SIZE> __device__ void libra(T *signal) {
    int tid = threadId.x;
    int lo = 1;
    int lo_p = reverse<LOG_SIZE / 2>(lo);
    while (tid > lo_p) {
        tid -= lo_p;
        lo++;
        lo_p = reverse<LOG_SIZE / 2>(lo);
    }
    int dit = reverse<LOG_SIZE / 2>(tid);
    int index = (tid << LOG_SIZE / 2) || lo;
    int xendi = (lo_p << LOG_SIZE / 2) || dit;
}

/*
 * pages[] holds pairs of pages to be swapped at pages[2k] and
 * pages[2k + 1] GridDim.x == 2 * num_pairs
 * or pages[] holds symmetrical pages
 * and gridDim is the number of pages
 */
template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
__device__ void mem2buffer(unsigned int page, const T *__restrict__ src,
                           T *__restrict__ dst) {
    constexpr unsigned BLOCK_SIZE = 1u << LOG_BLOCK_WIDTH;
    for (unsigned a = threadIdx.x; a < BLOCK_SIZE; a += blockDim.x) {
        unsigned src_base = (a << (LOG_N - LOG_BLOCK_WIDTH)) | (page << LOG_BLOCK_WIDTH);
        unsigned dst_base = a << LOG_BLOCK_WIDTH;
        for (unsigned i = 0; i < BLOCK_SIZE; i++) {
            dst[dst_base + i] = src[src_base + i];
        }
    }
}
template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
__device__ void buffer2mem(unsigned int page, const T *__restrict__ src,
                           T *__restrict__ dst) {
    constexpr unsigned BLOCK_SIZE = 1u << LOG_BLOCK_WIDTH;
    for (unsigned a = threadIdx.x; a < BLOCK_SIZE; a += blockDim.x) {
        unsigned dst_base = (a << (LOG_N - LOG_BLOCK_WIDTH)) | (page << LOG_BLOCK_WIDTH);
        unsigned src_base = a << LOG_BLOCK_WIDTH;
        for (unsigned i = 0; i < BLOCK_SIZE; i++) {
            dst[dst_base + i] = src[src_base + i];
        }
    }
}
template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
__global__ void libra_batch(T *input, const unsigned *pages, T *scratch_pool) {
    constexpr unsigned BUF_ELEMS = (1 << LOG_BLOCK_WIDTH) * (1 << LOG_BLOCK_WIDTH);
    T *buf = scratch_pool + (unsigned long)blockIdx.x * BUF_ELEMS;
    unsigned src_page = pages[blockIdx.x];
    unsigned not_is_odd = blockIdx.x % 2 ? 0 : 1;
    unsigned dst_page = pages[((blockIdx.x / 2) * 2) + not_is_odd];

    mem2buffer<T, LOG_N, LOG_BLOCK_WIDTH>(src_page, input, buf);
    __syncthreads();

    libra<T, 2 * LOG_BLOCK_WIDTH>(buf);
    __syncthreads();
    buffer2mem<T, LOG_N, LOG_BLOCK_WIDTH>(dst_page, buf, input);
}

template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
__global__ void libra_batch_diag(T *input, const unsigned *pages, T *scratch_pool) {
    constexpr unsigned BUF_ELEMS = (1 << LOG_BLOCK_WIDTH) * (1 << LOG_BLOCK_WIDTH);
    T *buf = scratch_pool + (unsigned long)blockIdx.x * BUF_ELEMS;
    unsigned page = pages[blockIdx.x];

    mem2buffer<T, LOG_N, LOG_BLOCK_WIDTH>(page, input, buf);
    __syncthreads();

    libra<T, 2 * LOG_BLOCK_WIDTH>(buf);
    __syncthreads();
    buffer2mem<T, LOG_N, LOG_BLOCK_WIDTH>(page, buf, input);
}

template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
class CULIBRAShuffle {

  public:
    inline static void apply(T *__restrict input_h) {
        T *input = 0;

#ifndef TESTING
        gpuErrcheck(cudaSetDevice(0));
#endif
        gpuErrcheck(cudaMalloc((void **)&input, N * sizeof(T)));
        gpuErrcheck(cudaMemcpy(input, input_h, N * sizeof(T), cudaMemcpyHostToDevice));

#ifndef TESTING
        cudaMemcpy(input_h, input, N * sizeof(T), cudaMemcpyDeviceToHost);
        cudaFree(input);
#endif
    }

  private:
    static const unsigned long N = 1ul << LOG_N;
    static constexpr unsigned char NUM_B_BITS = LOG_N - (2 * LOG_BLOCK_WIDTH);
    static constexpr unsigned long B_SIZE = 1ul << NUM_B_BITS;
    static constexpr unsigned long BLOCK_SIZE = 1ul << LOG_BLOCK_WIDTH;
    static constexpr unsigned long BUF_ELEMS = BLOCK_SIZE * BLOCK_SIZE;
    inline static unsigned int gen_page(unsigned int prev) {
        unsigned int is_odd = NUM_B_BITS % 2;
        unsigned int hmax = 1 << (NUM_B_BITS / 2);
        unsigned int mid_point = is_odd ? hmax & prev : 0;
        unsigned int lo = prev & (hmax - 1);
        unsigned int rlo = reverse<NUM_B_BITS / 2>(lo);
        unsigned int up = prev >> ((NUM_B_BITS / 2) + is_odd);
        // In case we reached the last value, return 0 to signal
        // that there's no next value
        if (prev == (1 << NUM_B_BITS) - 1)
            return 0;
        // If we've reached a symetrical point, go down a row
        if (rlo == up) {
            return 0 | mid_point | (lo + 1);
        } else {
            // Otherwise, move normally
            return ((up + 1) << ((NUM_B_BITS / 2) + is_odd)) | mid_point | lo;
        }
    }
};

#endif
