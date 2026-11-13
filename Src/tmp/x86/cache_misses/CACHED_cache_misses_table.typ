#let CACHED_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [COBRAShuffle], [1.87],
  [fCOBRA], [3.12],
  [SemiRecursiveShuffle], [5.96],
  [RecursiveShuffle], [5.96],
)
