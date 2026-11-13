#let NAIVE_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [LocalPairwiseShuffle], [1.79],
  [UnrolledShuffle], [0.67],
  [NaiveShuffle], [18.49],
  [XORShuffle], [1.98],
  [TableShuffle], [1.80],
)
