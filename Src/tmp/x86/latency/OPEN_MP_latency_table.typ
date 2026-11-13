#let OPEN_MP_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [2.97],
  [OpenMPSemiRecursiveShuffle], [3.79],
)
