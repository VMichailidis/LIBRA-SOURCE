#ifndef _CULIBRA_SHUFFLE_HPP
#define _CULIBRA_SHUFFLE_HPP
#include <cstdio>
#include <cstdlib>
#include <cuda_runtime.h>

#define THREAD_LIMIT 128

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

// Reverses the lowest BITS bits of x
template <unsigned char BITS>
__host__ __device__ inline static unsigned long reverse_bitwise(unsigned long x) {
    unsigned long res = 0;
#pragma unroll
    for (unsigned i = 0; i < BITS; ++i) {
        res = (res << 1) | ((x >> i) & 1ul);
    }
    return res;
}

template <typename T, unsigned char LOG_N> __global__ void libra(T *__restrict__ signal) {
    constexpr unsigned H_BITS = LOG_N / 2; // must be >= 1
    constexpr unsigned MID_BITS = LOG_N - 2 * H_BITS;
    constexpr unsigned H = 1u << H_BITS;

    const unsigned lo = blockIdx.x * blockDim.x + threadIdx.x; // contiguous in memory
    const unsigned u = blockIdx.y;
    const unsigned mid = blockIdx.z;

    if (lo >= H)
        return;
    const unsigned rlo = __brev(lo) >> (32 - H_BITS);
    if (u >= rlo)
        return; // swap each pair once
    const unsigned ru = __brev(u) >> (32 - H_BITS);

    const unsigned long index =
        ((unsigned long)u << (H_BITS + MID_BITS)) | ((unsigned long)mid << H_BITS) | lo;
    const unsigned long xendi =
        ((unsigned long)rlo << (H_BITS + MID_BITS)) | ((unsigned long)mid << H_BITS) | ru;

    T tmp = signal[index];
    signal[index] = signal[xendi];
    signal[xendi] = tmp;
}
template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
class CULIBRAShuffle {
  public:
    inline static void apply(T *__restrict input_h) {
        static_assert(LOG_N / 2 <= 15, "gridDim.y is limited to 65535");
        constexpr unsigned long N = 1ul << LOG_N;
        constexpr unsigned long H = 1ul << (LOG_N / 2);
        constexpr unsigned MID_BITS = LOG_N % 2;

        T *input = nullptr;
        gpuErrcheck(cudaSetDevice(0));
        gpuErrcheck(cudaMalloc((void **)&input, N * sizeof(T)));
#ifndef TESTING
        gpuErrcheck(cudaMemcpy(input, input_h, N * sizeof(T), cudaMemcpyHostToDevice));
#endif

        dim3 block(THREAD_LIMIT);
        dim3 grid((H + THREAD_LIMIT - 1) / THREAD_LIMIT, H, 1u << MID_BITS);

        libra<T, LOG_N><<<grid, block>>>(input);
        gpuErrcheck(cudaGetLastError());
        gpuErrcheck(cudaDeviceSynchronize());
#ifndef TESTING
        gpuErrcheck(cudaMemcpy(input_h, input, N * sizeof(T), cudaMemcpyDeviceToHost));
#endif
        gpuErrcheck(cudaFree(input));
    }
};
#endif
