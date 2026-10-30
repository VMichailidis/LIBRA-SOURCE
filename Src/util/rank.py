#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "matplotlib",
#     "pandas>=3.0.0",
#     "jinja2",
# ]
# ///
import argparse

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import argparse
import sys


parser = argparse.ArgumentParser()
parser.add_argument("-i", help="input file")
parser.add_argument("--lower", help="lower bound of comparisson")
parser.add_argument("--upper", help="upper bound of comparisson")
args = parser.parse_args()


def rank(series: pd.DataFrame) -> pd.DataFrame:
    series_p = series.sort_values(by=["N_BITS"], axis=0)
    # print(series_p)


if __name__ == "__main__":
    data = pd.read_csv(args.i)
    data_p = (
        data.loc[
            (data["N_BITS"] >= int(args.lower))
            & (data["N_BITS"] <= int(args.upper))
        ]
        .set_index("N_BITS")
        .rank(axis=1, method="min", ascending=True)
        .mean(axis=0)
        .rank(axis=0)
        .astype(int)
    )
    print(data_p)
