#include <bitset>
#include <cmath>
#include <iostream>
// TODO Write verification code
template <template <typename, unsigned char
#ifdef TEST_LOG_BLOCK_WIDTH
                    ,
                    unsigned char
#endif
                    > class Shuffler,
          unsigned char LOG_N
#ifdef TEST_LOG_BLOCK_WIDTH
          ,
          unsigned char LOG_BLOCK_WIDTH
#endif
          >
class Verifier {
  public:
    static void run() {

        printf("Testing for log(n) = %d\n", LOG_N);
        const unsigned long N = 1ul << LOG_N;
#ifndef GPU_TYPE
        unsigned long *testArray = new unsigned long[N];
#else
        unsigned long *testArray;
        cudaError_t status =
            cudaMallocHost((void **)&testArray, N * sizeof(unsigned long));
        if (status != cudaSuccess)
            printf("Error allocating pinned host memory\n");

#endif
        for (unsigned long i = 0; i < N; i++) {
            testArray[i] = i;
        }
        Shuffler<unsigned long, LOG_N
#ifdef TEST_LOG_BLOCK_WIDTH
                 ,
                 LOG_BLOCK_WIDTH
#endif
                 >::apply(testArray);
        int errors = 0;

        for (unsigned long i = 0; i < N; i++) {
            if (testArray[i] != _reverse(i, LOG_N)) {
                std::cout << "Expected: " << std::bitset<LOG_N>(_reverse(i, LOG_N))
                          << " got: " << std::bitset<LOG_N>(testArray[i]) << std::endl;
                errors++;
            }
        }
        if (errors == 0) {
            printf("\033[32mAll tests passed!\033[0m\n");

        } else {
            printf("\033[m%d tests failed!\033[0m\n", errors);
        }
    };

  private:
    static unsigned long _reverse(unsigned long x, unsigned long w) {
        unsigned long reversed = 0;
        for (unsigned long j = 0; j < w;
             j++) // log2(N) = 3 bits needed to represent indices
            reversed = (reversed << 1) | (x >> j & 1);
        return reversed;
    }
};
