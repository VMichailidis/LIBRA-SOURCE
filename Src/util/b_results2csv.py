# /// script
# requires-python = ">=3.14"
# dependencies = [
#     "pandas>=3.0.6",
# ]
# ///
import sys
import pandas as pd
import re

original_cols = [
    "method",
    "iterations",
    "N_BITS",
    "mean time",
    "min err",
    "max err",
]
if __name__ == "__main__":
    filenames = sys.argv[1:]
    data = [pd.read_csv(f, names=original_cols) for f in filenames]

    def extract_bsize(s):
        match = re.search(r"_LOGBC(\d+)", s)
        return int(match.group(1)) if match else None

    sizes = [extract_bsize(f) for f in filenames]
    data_b = [
        d.drop("method", axis=1).join(
            d["method"].apply(
                lambda name: (
                    f"{name} B = {s}"
                    if name != "LIBRAShuffle"
                    else "LIBRASuffle"
                )
            )
        )[original_cols]
        for d, s in zip(data, sizes)
    ]

    print(
        pd.concat(data_b)
        .pivot(index="N_BITS", columns="method", values="mean time")
        .to_csv()
    )
