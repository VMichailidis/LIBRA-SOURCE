#let NAIVE_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.85],
  [UnrolledShuffle], [0.71],
  [NaiveShuffle], [21.19],
  [XORShuffle], [2.24],
  [TableShuffle], [2.00],
)
