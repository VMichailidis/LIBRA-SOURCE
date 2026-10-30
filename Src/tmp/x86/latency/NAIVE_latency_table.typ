#let NAIVE_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.82],
  [UnrolledShuffle], [0.69],
  [NaiveShuffle], [17.95],
  [XORShuffle], [1.96],
  [TableShuffle], [1.91],
)
