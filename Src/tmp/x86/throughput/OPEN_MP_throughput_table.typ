#let OPEN_MP_throughput_table = table(
  columns: 2,
  [Algorithms], [avg $T_("LIBRA")/T_("algorithm")$], table.hline(),

  [OpenMPfCOBRA], [4.04],
  [OpenMPSemiRecursiveShuffle], [7.77],
)
