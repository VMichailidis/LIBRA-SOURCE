#ifndef SHUFFLER
#define SHUFFLER LIBRAShuffle
#include "../algorithms/LIBRAShuffle/LIBRAShuffle.hpp"
#endif
#include "Verify.hpp"
#include <complex>
#ifdef TEST_LOG_N
#ifdef TEST_LOG_BLOCK_WIDTH
int main() { Verifier<SHUFFLER, TEST_LOG_N, TEST_LOG_BLOCK_WIDTH>::run(); }
#endif
#endif

#ifdef TEST_LOG_N
#ifndef TEST_LOG_BLOCK_WIDTH
int main() { Verifier<SHUFFLER, TEST_LOG_N>::run(); }
#endif
#endif
