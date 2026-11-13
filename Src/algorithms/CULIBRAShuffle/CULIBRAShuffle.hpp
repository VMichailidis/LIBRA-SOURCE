#ifndef _CU_TLIBRA_SHUFFLE_HPP
#define _CU_TLIBRA_SHUFFLE_HPP
#include "../tLIBRAShuffle/tLIBRAShuffle.hpp" // gen_page must be public there
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cuda_runtime.h>
#include <vector>

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

// One block per queue entry (pageA, pageB = rev(pageA))
template <typename T, unsigned char LOG_N, unsigned char LBW>
__global__ void tlibra_kernel(T *__restrict__ data, const uint2 *__restrict__ queue) {
    constexpr unsigned B = 1u << LBW;
    constexpr unsigned ELEMS = B * B;
    constexpr unsigned STRIDE = B + 1; // padding against bank conflicts
    constexpr unsigned ROW_SHIFT = LOG_N - LBW;
    static_assert(2 * B * STRIDE * sizeof(T) <= 48 * 1024, "tiles exceed shared memory");

    __shared__ T tileA[B * STRIDE];
    __shared__ T tileB[B * STRIDE];

    const uint2 pair = queue[blockIdx.x];
    const size_t pageA = pair.x, pageB = pair.y;
    const bool self = (pageA == pageB);

    // Coalesced loads: consecutive threads read consecutive c
    for (unsigned i = threadIdx.x; i < ELEMS; i += blockDim.x) {
        const unsigned a = i >> LBW, c = i & (B - 1);
        const size_t row = (size_t)a << ROW_SHIFT;
        tileA[a * STRIDE + c] = data[row | (pageA << LBW) | c];
        if (!self)
            tileB[a * STRIDE + c] = data[row | (pageB << LBW) | c];
    }
    __syncthreads();

    // Coalesced stores; the transpose+reverse happens on the shared-memory read
    for (unsigned i = threadIdx.x; i < ELEMS; i += blockDim.x) {
        const unsigned a = i >> LBW, c = i & (B - 1);
        const unsigned ra = __brev(a) >> (32 - LBW);
        const unsigned rc = __brev(c) >> (32 - LBW);
        const unsigned src = rc * STRIDE + ra; // old[rev(c)][rev(a)]
        const size_t row = (size_t)a << ROW_SHIFT;
        if (self) {
            data[row | (pageA << LBW) | c] = tileA[src];
        } else {
            data[row | (pageB << LBW) | c] = tileA[src]; // old page A -> page B
            data[row | (pageA << LBW) | c] = tileB[src]; // old page B -> page A
        }
    }
}

template <typename T, unsigned char LOG_N, unsigned char LBW> class CULIBRAShuffle {
    static_assert(LBW >= 1 && LOG_N > 2 * LBW, "need at least one page bit");
    static constexpr unsigned NUM_B_BITS = LOG_N - 2 * LBW;

  public:
    static void apply(T *__restrict input_h) {
        constexpr unsigned long N = 1ul << LOG_N;
        constexpr unsigned B = 1u << LBW;
        constexpr unsigned THREADS = (B * B < 256) ? B * B : 256;

        const std::vector<uint2> queue = build_queue();
        assert(queue.size() < (1ul << 31));

        T *data_d = nullptr;
        uint2 *queue_d = nullptr;
        gpuErrcheck(cudaSetDevice(0));
        gpuErrcheck(cudaMalloc((void **)&data_d, N * sizeof(T)));
        gpuErrcheck(cudaMalloc((void **)&queue_d, queue.size() * sizeof(uint2)));
#ifdef TESTING
        gpuErrcheck(cudaMemcpy(data_d, input_h, N * sizeof(T), cudaMemcpyHostToDevice));
#endif
        gpuErrcheck(cudaMemcpy(queue_d, queue.data(), queue.size() * sizeof(uint2),
                               cudaMemcpyHostToDevice));

        tlibra_kernel<T, LOG_N, LBW>
            <<<(unsigned)queue.size(), THREADS>>>(data_d, queue_d);
        gpuErrcheck(cudaGetLastError());
        gpuErrcheck(cudaDeviceSynchronize());

#ifdef TESTING
        gpuErrcheck(cudaMemcpy(input_h, data_d, N * sizeof(T), cudaMemcpyDeviceToHost));
#endif
        gpuErrcheck(cudaFree(queue_d));
        gpuErrcheck(cudaFree(data_d));
    }

  private:
    static std::vector<uint2> build_queue() {
        using CPU = tLIBRAShuffle<T, LOG_N, LBW>;
        constexpr unsigned long P = 1ul << NUM_B_BITS;
        constexpr unsigned long FIXED = 1ul << ((NUM_B_BITS + 1) / 2); // b == rev(b)

        std::vector<uint2> q;
        q.reserve((P + FIXED) / 2);
        unsigned long b = 0, rb = 0;
        do {
            q.push_back(make_uint2((unsigned)b, (unsigned)rb));
            CPU::gen_page(b, rb);
        } while (b);

#ifndef NDEBUG
        // Every page must appear exactly once, and rb must be rev(b)
        std::vector<unsigned char> seen(P, 0);
        for (const uint2 &p : q) {
            unsigned r = 0;
            for (unsigned i = 0; i < NUM_B_BITS; ++i)
                r |= ((p.x >> i) & 1u) << (NUM_B_BITS - 1 - i);
            assert(r == p.y);
            ++seen[p.x];
            if (p.x != p.y)
                ++seen[p.y];
        }
        for (unsigned char s : seen)
            assert(s == 1);
#endif
        return q;
    }
};
#endif
