
#include "Clock.hpp"
#include "TestUtils.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <utility>
#include <valgrind/cachegrind.h>

#ifdef COBRA_GPU_TYPE
#include "cuda_runtime.h"
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
#endif
#ifdef GPU_TYPE
#include "cuda_runtime.h"
#endif
template <
#if defined(DEFAULT_TYPE) || defined(GPU_TYPE)
    template <typename, unsigned char> class Shuffler
#else
    template <typename, unsigned char, unsigned char> class Shuffler
#endif
    ,
    typename T, unsigned char LOG_N
#if defined(COBRA_TYPE) || defined(FCOBRA_TYPE) || defined(COBRA_GPU_TYPE)
    ,
    unsigned char LOG_BLOCK_WIDTH
#endif
#ifdef SEMI_REC_TYPE
    ,
    unsigned char RECURSIONS_REMAINING
#endif
    >
class TestRuntime {
  public:
    static void run(int runs, float quant) {
        const unsigned long N = 1ul << LOG_N;
#if defined(FCOBRA_TYPE)
        T *testArray = new T[N];
#elif defined(GPU_TYPE)
        T *testArray;
        cudaError_t status = cudaMallocHost((void **)&testArray, N * sizeof(T));
        if (status != cudaSuccess)
            printf("Error allocating pinned host memory\n");
#else
        T *testArray = new T[N];
#endif
        CACHEGRIND_START_INSTRUMENTATION;
#ifdef DEFAULT_TYPE
        Shuffler<T, LOG_N>::apply(testArray);
#endif
#ifdef COBRA_TYPE
        Shuffler<T, LOG_N, LOG_BLOCK_WIDTH>::apply(testArray);
#endif
#ifdef FCOBRA_TYPE
        Shuffler<T, LOG_N, LOG_BLOCK_WIDTH>::apply(testArray);
#endif
#ifdef SEMI_REC_TYPE
        Shuffler<T, LOG_N, RECURSIONS_REMAINING>::apply(testArray);
#endif
#ifdef GPU_TYPE
        Shuffler<T, LOG_N>::apply(testArray);
#endif
#ifdef COBRA_GPU_TYPE
        Shuffler<T, LOG_N, LOG_BLOCK_WIDTH>::apply(testArray);
#endif
        CACHEGRIND_STOP_INSTRUMENTATION;
    }
};
