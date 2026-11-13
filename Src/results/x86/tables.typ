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
  [UnrolledShuffle], [0.71],
  [NaiveShuffle], [21.19],
  [XORShuffle], [2.24],
  [TableShuffle], [2.00],
)
#let NAIVE_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.79],
  [UnrolledShuffle], [0.67],
  [NaiveShuffle], [18.49],
  [XORShuffle], [1.98],
  [TableShuffle], [1.80],
)
#let NAIVE_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.01],
  [UnrolledShuffle], [1.12],
  [NaiveShuffle], [1.00],
  [XORShuffle], [0.96],
  [TableShuffle], [1.14],
)
#let CACHED_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [COBRAShuffle], [3.02],
  [fCOBRA], [1.39],
  [SemiRecursiveShuffle], [3.09],
  [RecursiveShuffle], [3.29],
)
#let CACHED_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [COBRAShuffle], [3.08],
  [fCOBRA], [1.22],
  [SemiRecursiveShuffle], [2.64],
  [RecursiveShuffle], [2.78],
)
#let CACHED_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [COBRAShuffle], [1.87],
  [fCOBRA], [3.12],
  [SemiRecursiveShuffle], [5.96],
  [RecursiveShuffle], [5.96],
)
#let OPEN_MP_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [4.04],
  [OpenMPSemiRecursiveShuffle], [7.77],
)
#let OPEN_MP_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [2.97],
  [OpenMPSemiRecursiveShuffle], [3.79],
)
#let CUDA_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [CUDANaive], [1.06],
  [CUDAReverse], [1.64],
)
