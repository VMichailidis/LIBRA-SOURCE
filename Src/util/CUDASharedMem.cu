#include <cstdio>
#include <cuda_runtime.h>

int main() {
    cudaDeviceProp p;
    cudaGetDeviceProperties(&p, 0);
    printf("Device: %s\n", p.name);
    printf("Shared mem per block: %zu bytes\n", p.sharedMemPerBlock);
    int blocksPerSM;
    cudaDeviceGetAttribute(&blocksPerSM, cudaDevAttrMaxBlocksPerMultiprocessor, 0);
    printf("Max blocks per SM: %d\n", p.maxBlocksPerMultiProcessor);
    return 0;
}
