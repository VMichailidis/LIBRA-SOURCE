#include "Clock.hpp"
#include "TestUtils.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <utility>

#define LLC_EVICT_SIZE (40 * 1024)
#define ICACHE_EVICT_SIZE (64 * 1024)
void clear_cache(size_t size);
void *make_icache_evictor(size_t size);
void evict_icache(void *evictor);
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
// I had to move this here even though it is literally the same as
// utils in order to avoid compile errors
namespace utilsGpu {
double mean(double *values, int size) {
    double sum{0.0};
    for (int i = 0; i < size; ++i)
        sum += values[i];
    return (sum / size);
}

std::pair<int, int> get_indices(int size, float quantile) {
    float pos = size * quantile;
    std::pair<int, int> indices(floor(pos), ceil(pos));
    return indices;
}

std::pair<double, double> errors(double *values, int size, float quantile) {
    // sort values in-place
    std::sort(values, values + size);
    // determine indices of lower quantile
    std::pair<int, int> indices = get_indices(size, quantile);
    std::pair<double, double> errors;

    errors.first = (values[indices.first] + values[indices.second]) / 2.0;

    // now determine indices of upper quantile
    indices = get_indices(size, 1.0 - quantile);

    errors.second = (values[indices.first] + values[indices.second]) / 2.0;

    return errors;
}

} // namespace utilsGpu

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
        void *evictor = make_icache_evictor(ICACHE_EVICT_SIZE);
        Clock c;
        const unsigned long N = 1ul << LOG_N;
#if defined(FCOBRA_TYPE)
        T *testArray = new T[2 * N];
#elif defined(GPU_TYPE)
        T *testArray;
        cudaError_t status = cudaMallocHost((void **)&testArray, N * sizeof(T));
        if (status != cudaSuccess)
            printf("Error allocating pinned host memory\n");
#else
        T *testArray = new T[N];
#endif
        double *times = new double[runs];
        for (int i = 0; i < runs; ++i) {
            clear_cache(LLC_EVICT_SIZE);
            evict_icache(evictor);
            c.tick();
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
            times[i] = c.tock();
        };

        double mean_ = utils::mean(times, runs);

        std::pair<double, double> errors = utils::errors(times, runs, quant);

        utils::myassert(mean_, errors, times, runs);
        delete[] times;
        std::cout << mean_ << ", " << errors.first << ", " << errors.second << std::endl;
    }
};
