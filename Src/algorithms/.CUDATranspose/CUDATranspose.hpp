#include <assert.h>
#include <iostream>
#include <stdio.h>

inline cudaError_t checkCuda(cudaError_t result) {
#if defined(DEBUG) || defined(_DEBUG)
    if (result != cudaSuccess) {
        fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
        assert(result == cudaSuccess);
    }
#endif
    return result;
}

const int TILE_DIM = 32;
const int BLOCK_ROWS = 8;
const int NUM_REPS = 100;
template <typename T> __global__ void transposeNoBankConflicts(T *odata, const T *idata) {
    __shared__ T tile[TILE_DIM][TILE_DIM + 1];

    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;
    int width = gridDim.x * TILE_DIM;

    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS)
        tile[threadIdx.y + j][threadIdx.x] = idata[(y + j) * width + x];

    __syncthreads();

    x = blockIdx.y * TILE_DIM + threadIdx.x; // transpose block offset
    y = blockIdx.x * TILE_DIM + threadIdx.y;

    for (int j = 0; j < TILE_DIM; j += BLOCK_ROWS)
        odata[(y + j) * width + x] = tile[threadIdx.x][threadIdx.y + j];
}

template <typename T, unsigned char LOG_N> class CUDATranspose {
  public:
    inline static void apply(T *__restrict input_h) {
        const int nx = 1024;
        const int ny = 1024;
        const int mem_size = nx * ny * sizeof(T);

        dim3 dimGrid(nx / TILE_DIM, ny / TILE_DIM, 1);
        dim3 dimBlock(TILE_DIM, BLOCK_ROWS, 1);

        printf("Matrix size: %d %d, Block size: %d %d, Tile size: %d %d\n", nx, ny,
               TILE_DIM, BLOCK_ROWS, TILE_DIM, TILE_DIM);
        printf("dimGrid: %d %d %d. dimBlock: %d %d %d\n", dimGrid.x, dimGrid.y, dimGrid.z,
               dimBlock.x, dimBlock.y, dimBlock.z);
        int devId = 0;
        checkCuda(cudaSetDevice(devId));

        T *h_idata = (T *)malloc(mem_size);
        T *h_cdata = (T *)malloc(mem_size);
        T *h_tdata = (T *)malloc(mem_size);
        T *gold = (T *)malloc(mem_size);

        T *d_idata, *d_cdata, *d_tdata;
        checkCuda(cudaMalloc(&d_idata, mem_size));
        checkCuda(cudaMalloc(&d_cdata, mem_size));
        checkCuda(cudaMalloc(&d_tdata, mem_size));

        // check parameters and calculate execution configuration
        if (nx % TILE_DIM || ny % TILE_DIM) {
            printf("nx and ny must be a multiple of TILE_DIM\n");
            goto error_exit;
        }

        if (TILE_DIM % BLOCK_ROWS) {
            printf("TILE_DIM must be a multiple of BLOCK_ROWS\n");
            goto error_exit;
        }

        // device
        checkCuda(cudaMemcpy(d_idata, input_h, mem_size, cudaMemcpyHostToDevice));

        // ------------------------
        // transposeNoBankConflicts
        // ------------------------
        printf("%25s", "conflict-free transpose");
        checkCuda(cudaMemset(d_tdata, 0, mem_size));
        // warmup
        transposeNoBankConflicts<<<dimGrid, dimBlock>>>(d_tdata, d_idata);

        for (int i = 0; i < NUM_REPS; i++)
            transposeNoBankConflicts<<<dimGrid, dimBlock>>>(d_tdata, d_idata);
        checkCuda(cudaMemcpy(input_h, d_tdata, mem_size, cudaMemcpyDeviceToHost));

    error_exit:
        // cleanup

        checkCuda(cudaFree(d_tdata));
        checkCuda(cudaFree(d_cdata));
        checkCuda(cudaFree(d_idata));
        free(h_idata);
        free(h_tdata);
        free(h_cdata);
        free(gold);
    }
}
