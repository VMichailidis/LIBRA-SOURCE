#define XSTR(x) STR(x)
#define STR(x) #x
#include "cache_misses.hpp"
// THIS file takes the following inputs
// ALG: the algorihtm to be benchmarked
// DATATYPE: the type to be shuffled
// RUNS: the number of times the algorithm is run
// QUANT: the quantile to be output
// TEST_LOG_N: the LOG_N to be benchmarked
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
        TestRuntime<ALG, DATATYPE, LOG_N>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
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
        TestRuntime<ALG, DATATYPE, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
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
        TestRuntime<ALG, DATATYPE, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
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
        TestRuntime<ALG, DATATYPE, LOG_N, RECURSIONS_REMAINING>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
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
        TestRuntime<ALG, DATATYPE, LOG_N>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
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
        TestRuntime<ALG, DATATYPE, LOG_N, LOG_BLOCK_WIDTH>::run(RUNS, QUANT);
        std::cout << "algorithm: " << XSTR(ALG) << "\n"
                  << "runs: " << RUNS << "\n"
                  << "log_n: " << TEST_LOG_N << "\n";
    }
}
#endif
