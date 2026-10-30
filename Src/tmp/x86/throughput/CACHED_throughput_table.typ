#let CACHED_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [COBRAShuffle], [2.80],
  [fCOBRA], [1.09],
  [SemiRecursiveShuffle], [3.71],
  [RecursiveShuffle], [4.03],
)
