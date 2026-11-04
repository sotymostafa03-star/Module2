#!/usr/bin/env bash
# ==============================================================================
# PinjamLab Module 2: one command -- build, C++ tests, Python tests.
# MASTER 3.2 item 7: reproduce-from-clean.
#
# NOTE ON EXIT STATUS: unlike Module 1's script, this one FAILS when a toolchain
# is missing. Module 1's version printed [SUCCESS] even when it found no
# compiler and skipped the whole C++ half, which made a green banner worthless
# as evidence. Do not reintroduce that.
# ==============================================================================
set -euo pipefail

cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p bin

CXX="${CXX:-g++}"
PY="${PY:-python}"
command -v "$CXX" >/dev/null || { echo "FATAL: no C++ compiler ($CXX) on PATH."; exit 2; }
command -v "$PY"  >/dev/null || { echo "FATAL: no Python interpreter ($PY) on PATH."; exit 2; }

echo "================================================================================"
echo " [PinjamLab M2] $("$CXX" --version | head -1)"
echo " [PinjamLab M2] $("$PY" --version 2>&1)"
echo "================================================================================"

echo ""
echo ">>> [1/3] Building C++20 tests (-Wall -Wextra, warnings are defects)"
for t in test_structures test_catalog; do
    "$CXX" -std=c++20 -Wall -Wextra -Werror \
           -Icpp/include -Icpp/tests "cpp/tests/$t.cpp" -o "bin/$t"
done

echo ""
echo ">>> [2/3] Running C++20 test suites"
./bin/test_structures
./bin/test_catalog

echo ""
echo ">>> [3/3] Running Python 3.12 test suite (pytest)"
PYTHONPATH=py "$PY" -m pytest py/tests -q

echo ""
echo "================================================================================"
echo " [SUCCESS] C++ and Python suites both ran and both passed."
echo "================================================================================"
