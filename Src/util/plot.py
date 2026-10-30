#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "matplotlib",
#     "pandas>=3.0.0",
# ]
# ///
import argparse

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator
import numpy as np
import pandas as pd

colors = {
    "COBRAShuffle": "green",
    "CUDANaive": "green",
    "CUDAReverse": "orange",
    "CULIBRAShuffle": "teal",
    "fCOBRA": "orange",
    "tLIBRAShuffle": "teal",
    "LIBRAShuffle": "darkkhaki",
    "LocalPairwiseShuffle": "blue",
    "NaiveShuffle": "green",
    "NOOP": "green",
    "OpenMPfCOBRA": "orange",
    "OpenMPLIBRAShuffle": "teal",
    "OpenMPSemiRecursiveShuffle": "green",
    "RecursiveShuffle": "maroon",
    "SemiRecursiveShuffle": "hotpink",
    "TableShuffle": "brown",
    "UnrolledShuffle": "red",
    "XORShuffle": "purple",
}


def into_filesize(log2_value, suffix="B"):
    """Format 2**log2_value as a human readable quantity, e.g. 64KiB, 2GiB/s."""
    size = 2.0**log2_value
    for unit in ("", "Ki", "Mi", "Gi", "Ti"):
        if abs(size) < 1024.0:
            return f"{size:g}{unit}{suffix}"
        size /= 1024
    return f"{size:g}Pi{suffix}"


def into_seconds(log2_value, suffix="s"):
    size = 2.0**log2_value
    for unit in ("", "m", "μ", "n", "p"):
        if size > 1:
            return f"{size:.0f}{unit}{suffix}"
        size *= 1000
    return f"{size:.0f}a{suffix}"


def func(logn):
    n = 2**logn
    m = 49152.0
    b = 64
    b_inv = 1 / b
    result = (0.5 + b_inv) * n + np.sqrt(n) * b_inv - 0.5 * m
    result = result / n
    result[result < 0] = b_inv
    return result


def fmt_df(df):
    """
    bring dataframes to right format
    :return:
    """
    return df
    # df.columns = pd.to_numeric(df.columns)
    # df.columns.name = ""
    # return df.apply(pd.to_numeric, errors="coerce")


def df_preprocess(test_class, data, x_range, select):
    data_reduced = data[data["N_BITS"].between(x_range[0], x_range[1])]
    values = [
        col
        for col in data_reduced.columns
        if col != "N_BITS" and (select is None or col in select)
    ]
    selected_values = (
        [v for v in values if v in select] if select is not None else values
    )
    data_norm = data_reduced[["N_BITS"] + selected_values].copy()
    for value in selected_values:
        data_norm[value] = test_class.y_axis(
            data_reduced["N_BITS"], data_reduced[value]
        )
    return data_norm


class TestThroughput:
    name = "throughput"
    axis_name = "Throughput"

    def y_axis(logn: float, time: float):
        return np.log2((2**logn) / time)


class TestLatency:
    name = "latency"
    axis_name = "sec/Byte"

    def y_axis(logn: float, time: float):
        return np.log2(time / (2**logn))


class CacheMisses:
    name = "cache_misses"
    axis_name = "Cache Misses %"

    def y_axis(logn: float, cache_misses: float):
        # return np.log2(cache_misses / (2**logn))
        # print(cache_misses, logn)
        # cm = (cache_misses - float(args.miss_offset)).clip(lower=0.0)
        cm = cache_misses - float(args.miss_offset)
        # cm = cache_misses
        # return cm
        return (cm / (2**logn)) * 100


class BranchMisses:
    name = "branch_misses"
    axis_name = "Branch Misses %"

    def y_axis(logn: float, branch_misses: float):
        return branch_misses


def identify_test_class(testclass):
    match testclass:
        case "throughput":
            return TestThroughput
        case "latency":
            return TestLatency
        case "cache_misses":
            return CacheMisses
        case "branch_misses":
            return BranchMisses


def plot(
    data,
    select=None,
    range=None,
    min_err_path=None,
    max_err_path=None,
    filename=None,
    type="throughput",
    elem_size=1,
):
    testclass = identify_test_class(type)
    # print(range)
    if range != [] and range is not None:
        x_range = (int(range[0]), int(range[1]))
    else:
        x_range = (0, float("inf"))

    data_norm = df_preprocess(testclass, data, x_range, select)

    plt.figure(figsize=(10, 6))
    for d in filter(lambda x: not x == "N_BITS", data_norm):
        plt.plot(
            data_norm["N_BITS"],
            data_norm[d],
            label=d,
            color=colors[d] if d in colors else None,
        )
    # if type == "cache_misses":
    # plt.plot(
    #     data_norm["N_BITS"],
    #     float(args.miss_offset) / 2 ** (data_norm["N_BITS"]),
    #     label="O(2)",
    #     color="darkred",
    #     linestyle=":",
    #     linewidth=1,
    # )

    # plt.plot(
    #     data_norm["N_BITS"],
    #     float(args.miss_offset) / 2 ** (data_norm["N_BITS"]) + 0.015625,
    #     label=r"$O(2 + \frac{N}{B}$)",
    #     color="teal",
    #     linestyle=":",
    #     linewidth=1,
    # )

    # plt.plot(
    #     data_norm["N_BITS"],
    #     func(data_norm["N_BITS"]),
    #     label=r"$O((\frac{1}{2} + \frac{1}{B})N)$",
    #     color="green",
    #     linestyle=":",
    #     linewidth=1,
    # )

    ax = plt.gca()
    log_elem = np.log2(elem_size)
    ax.xaxis.set_major_formatter(
        FuncFormatter(lambda x, _: into_filesize(x + log_elem, "B"))
    )

    match type:
        case "throughput":
            ax.yaxis.set_major_locator(MaxNLocator(integer=True))
            ax.yaxis.set_major_formatter(
                FuncFormatter(lambda y, _: into_filesize(y + log_elem, "B/s"))
            )
        case "latency":
            ax.yaxis.set_major_locator(MaxNLocator(integer=True))
            ax.yaxis.set_major_formatter(
                FuncFormatter(lambda y, _: into_seconds(y))
            )

    plt.xlabel("Input size")
    plt.grid(True)
    plt.ylabel(testclass.axis_name)
    plt.legend()
    plt.tight_layout()
    if filename is None:
        plt.savefig(
            f"doc/images/logb_cpu{args.logb_cpu}_logb_gpu{args.logb_cu}_runs{args.runs}.png"
        )
    else:
        plt.savefig(filename)
        if args.to_csv:
            print("{")
            for d in data_norm:
                # TODO: fix trailing comma
                print(f'"{d}": ', list(data_norm[d].dropna()), ",")

            print("}")


parser = argparse.ArgumentParser()
parser.add_argument("--test_class", help="type of test to be plotted")
parser.add_argument("--data", help="Input file with mean time")
parser.add_argument("--min_err", help="Input file with max stdev")
parser.add_argument("--max_err", help="Input file with min stdev")
parser.add_argument("--logb_cpu", help="log(size) of cpu cache", default=0)
parser.add_argument("--logb_cu", help="log(size) of gpu cache", default=0)
parser.add_argument("--runs", help="Number of runs")
parser.add_argument("--miss_offset", help="baseline cache misses")
parser.add_argument(
    "--select", help="Select which benchmarks to show", nargs="*"
)
parser.add_argument("--filename", help="Output file name")
parser.add_argument("--to_csv", help="Export plot to csv", action="store_true")
parser.add_argument("--range", help="Range of values to plot", nargs="*")
args = parser.parse_args()


if __name__ == "__main__":
    plot(
        pd.read_csv(args.data),
        args.select,
        args.range,
        args.min_err,
        args.max_err,
        args.filename,
        args.test_class,
    )
    exit(0)
