#let NAIVE_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.85],
  [UnrolledShuffle], [0.75],
  [NaiveShuffle], [21.17],
  [XORShuffle], [2.20],
  [TableShuffle], [2.10],
)
