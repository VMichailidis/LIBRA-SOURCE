#let LIBRA_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

)
#let LIBRA_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

)
#let LIBRA_branch_misses_table = table(
  columns: 2,
  [Algorithms], [avg $B_("LIBRA")/B_("algorithm")$], table.hline(),

)
#let NAIVE_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.85],
  [UnrolledShuffle], [0.75],
  [NaiveShuffle], [21.17],
  [XORShuffle], [2.20],
  [TableShuffle], [2.10],
)
#let NAIVE_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.82],
  [UnrolledShuffle], [0.69],
  [NaiveShuffle], [17.95],
  [XORShuffle], [1.96],
  [TableShuffle], [1.91],
)
#let NAIVE_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.00],
  [UnrolledShuffle], [1.07],
  [NaiveShuffle], [1.00],
  [XORShuffle], [0.96],
  [TableShuffle], [1.11],
)
#let CACHED_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [COBRAShuffle], [2.80],
  [fCOBRA], [1.09],
  [SemiRecursiveShuffle], [3.71],
  [RecursiveShuffle], [4.03],
)
#let CACHED_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [COBRAShuffle], [2.81],
  [fCOBRA], [1.12],
  [SemiRecursiveShuffle], [3.32],
  [RecursiveShuffle], [3.56],
)
#let CACHED_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [COBRAShuffle], [1.87],
  [fCOBRA], [3.12],
  [SemiRecursiveShuffle], [5.71],
  [RecursiveShuffle], [5.71],
)
#let OPEN_MP_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [3.82],
  [OpenMPSemiRecursiveShuffle], [4.56],
)
#let OPEN_MP_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [3.22],
  [OpenMPSemiRecursiveShuffle], [4.38],
)
#let CUDA_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [CUDANaive], [0.86],
  [CUDAReverse], [1.56],
)
