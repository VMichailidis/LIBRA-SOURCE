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
    static_assert(LOG_N >= 2 && LOG_N / 2 <= 31, "unsupported size");
    constexpr unsigned H_BITS = LOG_N / 2;
    constexpr unsigned MID_BITS = LOG_N % 2;
    constexpr unsigned long H = 1ul << H_BITS;
    constexpr unsigned long TRI = H * (H - 1) / 2; // swaps per mid value

    const unsigned long t = (unsigned long)blockIdx.x * blockDim.x + threadIdx.x;
    if (t >= TRI) // only the last block has idle threads
        return;
    const unsigned long mid = blockIdx.y; // 0 when LOG_N is even

    // Invert t = r(r-1)/2 + u, with 0 <= u < r
    unsigned long r = (unsigned long)((1.0 + sqrt(1.0 + 8.0 * (double)t)) * 0.5);
    while (r * (r - 1) / 2 > t)
        --r; // fix rounding in either direction
    while (r * (r + 1) / 2 <= t)
        ++r;
    const unsigned long u = t - r * (r - 1) / 2;

    const unsigned long rr = __brev((unsigned)r) >> (32 - H_BITS); // rev(r)
    const unsigned long ru = __brev((unsigned)u) >> (32 - H_BITS); // rev(u)

    const unsigned long index = (u << (H_BITS + MID_BITS)) | (mid << H_BITS) | rr;
    const unsigned long xendi = (r << (H_BITS + MID_BITS)) | (mid << H_BITS) | ru;

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
        constexpr unsigned long TRI = H * (H - 1) / 2;

        T *input = nullptr;
        gpuErrcheck(cudaSetDevice(0));
        gpuErrcheck(cudaMalloc((void **)&input, N * sizeof(T)));
#ifndef TESTING
        gpuErrcheck(cudaMemcpy(input, input_h, N * sizeof(T), cudaMemcpyHostToDevice));
#endif

        dim3 block(THREAD_LIMIT);
        dim3 grid((TRI + THREAD_LIMIT - 1) / THREAD_LIMIT, 1u << (LOG_N % 2));
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
