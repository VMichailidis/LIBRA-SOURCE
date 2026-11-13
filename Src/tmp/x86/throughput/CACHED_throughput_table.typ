#let CACHED_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [COBRAShuffle], [3.02],
  [fCOBRA], [1.39],
  [SemiRecursiveShuffle], [3.09],
  [RecursiveShuffle], [3.29],
)
