#let CUDA_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [CUDANaive], [0.03],
  [CUDAReverse], [0.62],
)
