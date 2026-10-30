#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "jinja2>=3.1.6",
#     "matplotlib",
#     "pandas>=3.0.1",
# ]
# ///
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from pandas.io.formats import style
import argparse
import sys


def df_preprocess(data, x_range, select, denom):
    data_reduced = data[data["N_BITS"].between(x_range[0], x_range[1])]
    values = [
        col
        for col in data_reduced.columns
        if col != "N_BITS" and (select is None or col in select)
    ]
    data_norm = data_reduced[["N_BITS"] + values].copy()
    for value in values:
        # data_norm[value] = (2 ** data_reduced["N_BITS"]) / data_reduced[value]
        data_norm[value] = data_reduced[denom] / data_reduced[value]

    return data_norm


parser = argparse.ArgumentParser()
parser.add_argument("--data", help="Input file with mean time")
parser.add_argument(
    "--range",
    help="range to plot",
    default=(0, float("inf")),
    nargs="+",
)
parser.add_argument(
    "--select",
    help="Select which benchmarks to show",
    nargs="+",
)
parser.add_argument(
    "--denominator", help="Algorithm to compare against", default="LIBRAShuffle"
)
parser.add_argument(
    "--symbol", help="The sybmol of the type of data placed on the table"
)
parser.add_argument("--name", help="the name of the table")
args = parser.parse_args()

if __name__ == "__main__":
    x_range = (
        (int(args.range[0]), int(args.range[1]))
        if range is not None
        else (0, float("inf"))
    )
    data = (
        df_preprocess(
            pd.read_csv(args.data),
            x_range,
            args.select.append(args.denominator),
            args.denominator,
        )
        .drop("N_BITS", axis=1)
        .mean(axis=0)
    )
    data.name = "Speedup"
    dt_tmp = pd.read_csv(args.data)
    dt_tmp0 = dt_tmp[dt_tmp["N_BITS"].between(x_range[0], x_range[1])]
    dt_tmp1 = (
        dt_tmp0.iloc[:, 1:].div(2 ** dt_tmp["N_BITS"], axis=0).mean(axis=0)
    )

    dt_tmp2 = dt_tmp1 * 5.5e9
    dt_tmp2.name = "Clocks per element"
    dt_tmp3 = pd.merge(
        data,
        dt_tmp2,
        left_index=True,
        right_index=True,
    )

    pd.options.display.float_format = "{:,.2f}".format
    raw_table = (
        pd.DataFrame(data[args.denominator] / data[args.select])
        .drop(args.denominator)
        # t_tmp3
        .style.format("{:.2f}")
        .to_typst()
        .split("\n")
    )
    # header = f'  [Algorithms], [avg ${args.symbol}_("LIBRA")/{args.symbol}_("algorithm")$], [Clock Cycles per Element]\n  table.hline(),'
    header = f'  [Algorithms], [avg ${args.symbol}_("LIBRA")/{args.symbol}_("algorithm")$], table.hline(),'
    first_line = f"#let {args.name} = table("
    table = "\n".join([first_line] + [raw_table[1]] + [header] + raw_table[3:])

    print(table)
