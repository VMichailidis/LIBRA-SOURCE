#let NAIVE_cache_misses_table = table(
  columns: 2,
  [Algorithms], [avg $C_("LIBRA")/C_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.01],
  [UnrolledShuffle], [1.12],
  [NaiveShuffle], [1.00],
  [XORShuffle], [0.96],
  [TableShuffle], [1.14],
)
