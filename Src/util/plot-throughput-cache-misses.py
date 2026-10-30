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


class TestThroughput:
    name = "throughput"
    title = "Throughput"
    axis_name = "Throughput"

    def y_axis(logn: float, time: float):
        return np.log2((2**logn) / time)


class TestLatency:
    name = "latency"
    title = "Latency"
    axis_name = "log(t) (sec) / elem"

    def y_axis(logn: float, time: float):
        return np.log2(time / (2**logn))


class CacheMisses:
    name = "cache_misses"
    title = "Cache Misses"
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
    title = "Branch Misses"
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


def df_preprocess(test_class_name, data, x_range, select):
    test_class = identify_test_class(test_class_name)
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
        data_norm[value + " (" + test_class.title + ")"] = test_class.y_axis(
            data_reduced["N_BITS"], data_reduced[value]
        )
        del data_norm[value]
    return data_norm


def into_filesize(size, suffix):
    size = 2**size
    for unit in ("", "Ki", "Mi", "Gi", "Ti"):
        if abs(size) < 1024.0:
            return f"{size:3.0f}{unit}{suffix}"
        size /= 1024
    return f"{size:3.1f}Pi{suffix}"


def plot(throughput, cache_misses, select, filename, range=None, elem_size=1):
    if range != [] and range is not None:
        x_range = (int(range[0]), int(range[1]))
    else:
        x_range = (0, float("inf"))

    log_elem = np.log2(elem_size)
    th_norm = df_preprocess("throughput", throughput, x_range, select)
    ch_norm = df_preprocess("cache_misses", cache_misses, x_range, select)
    fig, ax_th = plt.subplots(figsize=(10, 6))
    ax_th.set_xlabel("Input size")
    ax_th.set_ylabel(TestThroughput.axis_name)
    ax_th.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax_th.yaxis.set_major_locator(MaxNLocator(integer=True))
    ax_th.xaxis.set_major_formatter(
        FuncFormatter(lambda x, _: into_filesize(x + log_elem, "B"))
    )
    ax_th.yaxis.set_major_formatter(
        FuncFormatter(lambda y, _: into_filesize(y + log_elem, "B/s"))
    )

    ax_ch = ax_th.twinx()
    ax_ch.set_ylabel(CacheMisses.axis_name)

    for d in filter(lambda x: not x == "N_BITS", th_norm):
        ax_th.plot(th_norm["N_BITS"], th_norm[d], label=d, color="purple")

    for d in filter(lambda x: not x == "N_BITS", ch_norm):
        ax_ch.plot(ch_norm["N_BITS"], ch_norm[d], label=d, color="teal")

    fig.legend(loc="outside upper right", ncol=2)
    fig.tight_layout()
    fig.savefig(filename, dpi=300)


parser = argparse.ArgumentParser()
parser.add_argument("--throughput", help="Input file with mean throughput")
parser.add_argument("--cache_misses", help="Input file with mean cache misses")

parser.add_argument("--miss_offset", help="baseline cache misses", default=2)
parser.add_argument(
    "--select",
    help="Select which benchmarks to show",
    nargs="*",
    default=["LIBRAShuffle"],
)
parser.add_argument("--output", "-o", help="Output file name")
parser.add_argument(
    "--range", help="Range of values to plot", nargs="*", default=[4, 28]
)
args = parser.parse_args()


if __name__ == "__main__":
    plot(
        pd.read_csv(args.throughput),
        pd.read_csv(args.cache_misses),
        args.select,
        args.output,
    )
    exit(0)
