#!/usr/bin/env bash
# parse_perf.sh — parses `perf stat -r N` output from stdin
# Usage: perf stat -r 1000 -e cpu_core/cache-misses/ taskset -c 0 ./mybenchmark 2>&1 | ./parse_perf.sh

# set -euo pipefail
LC_NUMERIC=en_US.utf8
OUT=$(cat)
# Strip commas so the numbers are plain integers
strip_commas() { sed 's/,//g'; }
# RUNS=$(grep -oP '\(\K[0-9]+(?= runs\))' <<< "$OUT")
LLD_MISSES=$(grep -oP '(?<=D1  misses:)\s*[0-9,]+' <<< "$OUT" | strip_commas | tr -d ' ')
# MISSES=$(grep -oP '^\s*\K[0-9]+(?=\s+cpu_core/L1-dcache-load-misses/)' <<< "$OUT")
# STDEV=$(grep -oP 'L1-dcache-load-misses.*\+-\s*\K[0-9]+,[0-9]+(?=%)' <<< "$OUT" | sed 's/,/./')
# ALG=$(grep -m 1 -oP '(?<=algorithm:\s)\w+' <<< "$OUT")
# LOG_N=$(grep -m 1 -oP '(?<=log_n:\s)\w+' <<< "$OUT")
# RUNS=$(grep -m 1 -oP '(?<=runs:\s)\w+' <<< "$OUT")
# Program name from the "Command:" line
PROG=$(grep -oP '(?<=Command: )\S+' <<< "$OUT" | sed 's|.*/||')
 
# Program-printed metadata (from the benchmark's own stdout, interleaved in $OUT)
ALG=$(grep -m 1 -oP '(?<=algorithm:\s)\w+' <<< "$OUT")
LOG_N=$(grep -m 1 -oP '(?<=log_n:\s)\w+' <<< "$OUT")
RUNS=$(grep -m 1 -oP '(?<=runs:\s)\w+' <<< "$OUT")

printf "%s, %s, %s, %s\n" "$ALG" "$RUNS" "$LOG_N" "$LLD_MISSES"
