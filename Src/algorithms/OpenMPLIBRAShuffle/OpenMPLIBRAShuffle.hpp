#ifndef _OPENMPLIBRASHUFFLE_
#define _OPENMPLIBRASHUFFLE_
#include <algorithm>
#include <cstring>
#include <iostream>
#include <omp.h>
#include <vector>
// TODO: re-write all methods to be more templated
// TODO: migrate to library reverse function
template <unsigned char LOG_N>
inline static void advance_index_and_reversed(unsigned long &index,
                                              unsigned long &reversed) {
    unsigned long temp = index + 1;
    unsigned long tail = (index ^ temp);
    // tail is of the form 00...011...1
    // It is a mask representing the number of bitflips
    index = temp;
    // create the reverse of tail, which is of form 11...100...0:
    //
    // Because when incrementing a number the number of bitflips
    // is a continuous string of 1's until cout is zero
    // We can simply shift the mask to the left and have the
    // reverse of the bitflip mask
    auto shift = __builtin_clzl(tail);
    tail <<= shift;
    tail >>= ((sizeof(unsigned long) * 8) - LOG_N);

    // xor reversed with reversed tail gives reversed of index+1:
    // The bitflips can be propageted via a simple xor
    reversed ^= tail;
}
template <typename T, unsigned char LOG_N, unsigned char LOG_BLOCK_WIDTH>
class OpenMPLIBRAShuffle {
  public:
    inline static void apply(T *__restrict const v) {
        unsigned long b = 0;
        unsigned long rb = 0;
        // The number of iterations to calculate the bit reveral
        // derived from (2^b  + 2^(b/2))/2 for even b and
        // 2^(b-1) + 2^((b-1)/2) for odd b
        // We need to include the case where b = b'
        constexpr unsigned ITERATIONS =
            ((1 << 2 * (NUM_B_BITS / 2)) + (1 << NUM_B_BITS / 2)) / 2 << NUM_B_BITS % 2;
        constexpr unsigned THREADS = ITERATIONS > 20 ? ITERATIONS : 20;
        unsigned long pages[ITERATIONS][2];
        // printf("ITERATIONS: %d\n", ITERATIONS);
        for (unsigned long i = 0; i < ITERATIONS; i++) {
            pages[i][0] = b;
            pages[i][1] = rb;
            gen_page(b, rb);
        }
        T *buffers[THREADS];
        for (unsigned i = 0; i < THREADS; i++) {
            buffers[i] =
                (T *)(std::malloc(sizeof(T) * (1 << LOG_BLOCK_WIDTH << LOG_BLOCK_WIDTH)));
        }
#pragma omp parallel for num_threads(THREADS)
        for (unsigned long i = 0; i < ITERATIONS; i++) {
            int tid = omp_get_thread_num();
            // printf("%d: %d, %d\n", omp_get_thread_num(),
            // pages[i][0],
            //        pages[i][1]);
            if (pages[i][0] != pages[i][1]) {
                array2buffer(v, pages[i][0], buffers[tid]);
                even(buffers[tid]);
                swap_buffer_array(buffers[tid], pages[i][1], v);
                even(buffers[tid]);
                buffer2array(buffers[tid], pages[i][0], v);

            } else {
                array2buffer(v, pages[i][0], buffers[tid]);
                even(buffers[tid]);
                buffer2array(buffers[tid], pages[i][0], v);
            }
        }

        for (int i = 0; i < THREADS; i++) {
            free(buffers[i]);
        }
        return;
    }

  private:
    static constexpr unsigned char NUM_B_BITS = LOG_N - (2 * LOG_BLOCK_WIDTH);
    static constexpr unsigned long B_SIZE = 1ul << NUM_B_BITS;
    static constexpr unsigned long BLOCK_SIZE = 1ul << LOG_BLOCK_WIDTH;
    inline static void even(T *__restrict const v) {
        unsigned long rlo = 0;
        unsigned long ru = 0;
        for (unsigned long lo = 0; lo < (1 << (LOG_BLOCK_WIDTH));) {

            for (unsigned long u = 0; u < rlo;) {
                unsigned long index = (u << (LOG_BLOCK_WIDTH)) | lo;
                unsigned long xendi = (rlo << (LOG_BLOCK_WIDTH)) | ru;

                std::swap(v[index], v[xendi]);
                advance_index_and_reversed<LOG_BLOCK_WIDTH>(u, ru);
            }
            ru = 0;
            advance_index_and_reversed<LOG_BLOCK_WIDTH>(lo, rlo);
        }
    }
    inline static void odd(T *__restrict const v) {
        unsigned long rlo = 0;
        unsigned long ru = 0;
        for (unsigned long lo = 0; lo < 1 << (LOG_BLOCK_WIDTH);) {
            for (unsigned long u = 0; u < rlo;) {
                unsigned long index = (u << ((LOG_BLOCK_WIDTH) + 1)) | lo;
                unsigned long xendi = (rlo << ((LOG_BLOCK_WIDTH) + 1)) | ru;
                std::swap(v[index], v[xendi]);
                advance_index_and_reversed<LOG_BLOCK_WIDTH>(u, ru);
            }
            ru = 0;
            advance_index_and_reversed<LOG_BLOCK_WIDTH>(lo, rlo);
        }
        rlo = 0;
        ru = 0;
        for (unsigned long lo = 0; lo < 1 << (LOG_BLOCK_WIDTH);) {
            for (unsigned long u = 0; u < rlo;) {
                unsigned long int index =
                    (u << ((LOG_BLOCK_WIDTH) + 1)) | 1 << LOG_BLOCK_WIDTH | lo;
                unsigned long int xendi =
                    (rlo << ((LOG_BLOCK_WIDTH) + 1)) | 1 << LOG_BLOCK_WIDTH | ru;
                std::swap(v[index], v[xendi]);
                advance_index_and_reversed<LOG_BLOCK_WIDTH>(u, ru);
            }
            ru = 0;
            advance_index_and_reversed<LOG_BLOCK_WIDTH>(lo, rlo);
        }
    }
    inline static void array2buffer(T array[], unsigned int page, T buffer[]) {
        for (int a = 0; a < BLOCK_SIZE; a++) {
            std::memcpy(&buffer[a << LOG_BLOCK_WIDTH],
                        &array[a << (LOG_N - LOG_BLOCK_WIDTH) | page << LOG_BLOCK_WIDTH],
                        sizeof(T) * BLOCK_SIZE);
        }
    }
    inline static void buffer2array(T buffer[], unsigned int page, T array[]) {
        for (int a = 0; a < BLOCK_SIZE; a++) {
            std::memcpy(&array[a << (LOG_N - LOG_BLOCK_WIDTH) | page << LOG_BLOCK_WIDTH],
                        &buffer[a << LOG_BLOCK_WIDTH], sizeof(T) * BLOCK_SIZE);
        }
    }
    inline static void swap_buffer_array(T buffer[], unsigned int page, T array[]) {
        for (int a = 0; a < BLOCK_SIZE; a++) {
            for (int c = 0; c < BLOCK_SIZE; c++) {
                std::swap(
                    array[a << (LOG_N - LOG_BLOCK_WIDTH) | page << LOG_BLOCK_WIDTH | c],
                    buffer[a << LOG_BLOCK_WIDTH | c]);
            }
        }
    }
    inline static void gen_page(unsigned long &prev, unsigned long &rprev) {
        constexpr unsigned long is_odd = NUM_B_BITS % 2;
        constexpr unsigned long hwidth = NUM_B_BITS / 2;
        constexpr unsigned long hmax = 1 << (NUM_B_BITS / 2);
        constexpr unsigned long t = ~hmax & ((1ul << NUM_B_BITS) - 1);

        unsigned long mid_point = is_odd ? hmax & prev : 0;

        unsigned long lo = prev & (hmax - 1);
        unsigned long rlo = rprev >> (hwidth + is_odd);
        unsigned long up = prev >> (hwidth + is_odd);
        unsigned long rup = rprev & (hmax - 1);

        // In case we reached the last value, return
        // 0 to signal that there's no next value
        if (prev == (1 << NUM_B_BITS) - 1) {
            // overflow to 0 once all combinations have been generated
            prev = 0;
            rprev = 0;
        } else if (prev == t && is_odd) {
            // overflow to 00..00100..00 once 11..11011..11 is reached
            prev = hmax;
            rprev = hmax;
        } else if (rlo == up) {
            // If we've reached a symetrical point, go down a row
            advance_index_and_reversed<NUM_B_BITS / 2>(lo, rlo);
            prev = 0 << (hwidth + is_odd) | mid_point | lo;
            rprev = rlo << (hwidth + is_odd) | mid_point | 0;
        } else {
            advance_index_and_reversed<NUM_B_BITS / 2>(up, rup);
            // Otherwise, move normally
            prev = up << (hwidth + is_odd) | mid_point | lo;
            rprev = rlo << (hwidth + is_odd) | mid_point | rup;
        }
    }
};

#endif
