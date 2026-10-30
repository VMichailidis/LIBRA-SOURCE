#!/usr/bin/env bash
LC_NUMERIC=en_US.utf8
OUT=$(cat)
# Strip commas so the numbers are plain integers
strip_commas() { sed 's/,//g'; }
strip_percent() { sed 's/%//g'; }
BRANCH_MISSES=$(grep -oP '(?<=Mispred rate:)\s*[0-9,.]+' <<< "$OUT" | strip_percent | tr -d ' ')
PROG=$(grep -oP '(?<=Command: )\S+' <<< "$OUT" | sed 's|.*/||')
 
# Program-printed metadata (from the benchmark's own stdout, interleaved in $OUT)
ALG=$(grep -m 1 -oP '(?<=algorithm:\s)\w+' <<< "$OUT")
LOG_N=$(grep -m 1 -oP '(?<=log_n:\s)\w+' <<< "$OUT")
RUNS=$(grep -m 1 -oP '(?<=runs:\s)\w+' <<< "$OUT")

printf "%s, %s, %s, %s\n" "$ALG" "$RUNS" "$LOG_N" "$BRANCH_MISSES"
