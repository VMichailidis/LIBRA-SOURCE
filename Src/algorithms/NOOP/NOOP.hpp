#ifndef _NOOP_HPP
#define _NOOP_HPP

#include "../../util/BitReversal.hpp"
#include <algorithm>

// THIS ALGORITHM IS MEANT CALCULATE THE CACHE MISSES CAUSED BY EXECUTING A PROGRAM
template <typename T, unsigned char LOG_N> class NOOP {
  public:
    __attribute__((noinline)) static void apply(T *__restrict const v) {
        constexpr unsigned long int N = 1ul << LOG_N;
    }
};

#endif
