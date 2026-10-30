#let OPEN_MP_latency_table = table(
  columns: 2,
  [Algorithms], [avg $L_("LIBRA")/L_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [3.22],
  [OpenMPSemiRecursiveShuffle], [4.38],
)
