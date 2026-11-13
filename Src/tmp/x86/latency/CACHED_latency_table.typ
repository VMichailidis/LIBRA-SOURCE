#let CACHED_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [COBRAShuffle], [3.08],
  [fCOBRA], [1.22],
  [SemiRecursiveShuffle], [2.64],
  [RecursiveShuffle], [2.78],
)
