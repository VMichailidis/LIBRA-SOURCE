#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "matplotlib",
#     "pandas>=3.0.0",
# ]
# ///
import sys
import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(2, 28, 10**6)
y = (x - np.sqrt(x)) / (2 * x)
plt.xlabel("N")
plt.ylabel("Visited Elements / Total Elements")
plt.plot(x, y)
# plt.show()
plt.savefig(sys.argv[1])
