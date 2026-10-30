#!/usr/bin/env bash

set -euo pipefail

CORES=($1); shift
[[ "$1" == "--" ]] && shift

LOCKDIR="${LOCKDIR:-/tmp/core_locks}"
mkdir -p "$LOCKDIR"
for core in "${CORES[@]}"; do
    : > "$LOCKDIR/core_${core}.lock"   # ensure files exist before we watch the dir
done

# fd 3 is just a signaling channel local to this function's caller; simpler: use return code
attempt_sweep() {
    for core in "${CORES[@]}"; do
        local lockfile="$LOCKDIR/core_${core}.lock"
        exec {fd}>"$lockfile"
        if flock -n "$fd"; then
            ACQUIRED_CORE="$core"
            ACQUIRED_FD="$fd"
            return 0
        fi
        exec {fd}>&-
    done
    return 1
}

# Try immediately first — covers the common case with zero inotify overhead
if attempt_sweep; then
    exec taskset -c "$ACQUIRED_CORE" "$@"
fi

# Nothing free yet — watch the lock dir and re-sweep every time any lock file closes
while true; do
    inotifywait -t 1 -q -e close --format '%f' "$LOCKDIR" >/dev/null 2>&1 || true
    if attempt_sweep; then
        exec taskset -c "$ACQUIRED_CORE" "$@"
    fi
    # spurious wakeup (e.g. someone else's failed non-blocking attempt closed their fd
    # right as we opened ours) — loop back and wait for the next close event
done
