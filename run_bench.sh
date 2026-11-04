#!/usr/bin/env bash
# ==============================================================================
# PinjamLab Module 2: the four-way benchmark, one command, CSV out.
#
# -O2, never the sanitizer build: timing a debug build proves nothing
# (Module 2, "Measuring Honestly", rule 1).
#
# The linear scan at n = 1e6 is genuinely slow -- ~26 s per repeat, 7 repeats,
# three implementations. Expect this to take several minutes. That wait is
# itself the lesson.
# ==============================================================================
set -euo pipefail

cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p bin

CXX="${CXX:-g++}"
PY="${PY:-python}"
OUT="sample-data/bench-results.csv"

echo "Building the benchmark at -O2..."
"$CXX" -std=c++20 -O2 -Wall -Wextra -Icpp/include cpp/bench/main.cpp -o bin/bench

echo "Machine: $(uname -s -m 2>/dev/null || echo unknown)"
echo "Compiler: $("$CXX" --version | head -1)"
echo "Python:   $("$PY" --version 2>&1)"
echo "Running... (minutes, not seconds)"

{
    ./bin/bench
    PYTHONPATH=py "$PY" py/bench/bench_lookup.py | tail -n +2
} > "$OUT"

echo ""
echo "Wrote $OUT:"
cat "$OUT"
echo ""
echo "Commit that file. It is evidence, and your partner will rerun it."
