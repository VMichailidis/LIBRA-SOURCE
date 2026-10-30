#!/usr/bin/env bash

# set -euo pipefail
LC_NUMERIC=en_US.utf8
OUT=$(cat)

MISSES=$(grep -oP '^\s*\K[0-9]+(?=\s+cpu_core/L1-dcache-load-misses/)' <<< "$OUT")

printf "%s\n" "$MISSES"
