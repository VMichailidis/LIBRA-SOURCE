#define XSTR(x) STR(x)
#define STR(x) #x
#include "latency.hpp"
// THIS file takes the following inputs
// ALG: the algorihtm to be benchmarked
// DATATYPE: the type to be shuffled
// RUNS: the number of times the algorithm is run
// QUANT: the quantile to be output
// TEST_LOG_N: the LOG_N to be benchmarked
void clear_cache(size_t size) {
    volatile char *buf = (char *)malloc(size);
    for (size_t i = 0; i < size; i += 64) {
        buf[i] = (char)(buf[i] + 1); // force read+write, defeats optimization
    }
    free((char *)buf);
}
void *make_icache_evictor(size_t size) {
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    memset(mem, 0x90, size);                 // ox90 NOP
    ((unsigned char *)mem)[size - 1] = 0xC3; // x86 RETN
    return mem;
}
void evict_icache(void *evictor) {
    void (*junk)(void) = (void (*)(void))evictor;
    junk();
}

#ifdef DEFAULT_TYPE
#ifndef ALG
#define ALG NaiveShuffle
#include "../../algorithms/NaiveShuffle/NaiveShuffle.hpp"
#endif
// Input method is included via an include flag
#include <complex>
#include <iostream>
int main() {
    {
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, std::complex<DATATYPE>, LOG_N>::run(RUNS, QUANT);
    }
}
#endif
#ifdef COBRA_TYPE
#include <complex>
#include <iostream>

int main() {
    {
        const unsigned char LOG_BLOCK_WIDTH = TEST_LOG_BLOCK_WIDTH;
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, std::complex<DATATYPE>, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS,
                                                                              QUANT);
    }
}
#endif
#ifdef FCOBRA_TYPE
#include <complex>
#include <iostream>

int main() {
    {
        const unsigned char LOG_BLOCK_WIDTH = TEST_LOG_BLOCK_WIDTH;
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, DATATYPE, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS, QUANT);
    }
}
#endif
#ifdef SEMI_REC_TYPE
#include <complex>
#include <iostream>
int main() {
    {
        const unsigned char RECURSIONS_REMAINING = TEST_RECURSIONS_REMAINING;
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, std::complex<DATATYPE>, LOG_N, RECURSIONS_REMAINING>::run(RUNS,
                                                                                   QUANT);
    }
}
#endif
#ifdef GPU_TYPE
#include <complex>
#include <cuda_runtime.h>
#include <driver_types.h>
#include <iostream>

int main() {
    {
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, std::complex<DATATYPE>, LOG_N>::run(RUNS, QUANT);
    }
}
#endif
#ifdef COBRA_GPU_TYPE
#include <complex>
#include <cuda_runtime.h>
#include <driver_types.h>
#include <iostream>

int main() {
    {
        const unsigned char LOG_BLOCK_WIDTH = TEST_LOG_BLOCK_WIDTH;
        const unsigned char LOG_N = TEST_LOG_N;
        std::cout << XSTR(ALG) << ", " << RUNS << ", " << TEST_LOG_N << ", ";
        TestRuntime<ALG, std::complex<DATATYPE>, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS,
                                                                              QUANT);
    }
}
#endif
