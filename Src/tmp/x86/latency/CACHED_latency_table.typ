#let CACHED_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [COBRAShuffle], [2.81],
  [fCOBRA], [1.12],
  [SemiRecursiveShuffle], [3.32],
  [RecursiveShuffle], [3.56],
)
