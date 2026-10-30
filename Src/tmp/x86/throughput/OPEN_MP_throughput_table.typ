#let OPEN_MP_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [3.82],
  [OpenMPSemiRecursiveShuffle], [4.56],
)
