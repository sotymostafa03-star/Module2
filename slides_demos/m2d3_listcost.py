"""Week 5 slide demo -- Lab 2.2's honest finding, measured.

Times `contains()` on the hand-built LinkedList against the built-in `list`
on 10,000 elements, as Lab 2.2 requires. Both are O(n) scans, so complexity
predicts a tie; the measurement does not, and the gap is the point.

Searches for a value that is ABSENT, so both walk the whole structure -- a
present key would let one of them stop early and measure nothing useful.

NOT a live slide block: this is a timing, and a timing run under Pyodide in a
browser would not be the number Lab 2.2 asks students to reproduce natively.

Run:  PYTHONPATH=py python slides_demos/m2d3_listcost.py
"""
import statistics
import sys
import time

from pinjamlab.structures.linked_list import LinkedList

N = 10_000
REPEATS = 7
MISSING = "ABSENT-999"


def timed(target, label: str) -> float:
    samples = []
    for _ in range(REPEATS + 1):                 # +1 warm-up, discarded below
        t0 = time.perf_counter()
        found = MISSING in target
        samples.append((time.perf_counter() - t0) * 1e6)
        assert not found
    median = statistics.median(samples[1:])      # drop the warm-up
    print(f"  {label:<28} {median:9.1f} us per full scan")
    return median


tags = [f"LP-{i:05d}" for i in range(N)]
builtin = list(tags)
handbuilt = LinkedList(tags)

print(f"Python {sys.version.split()[0]} -- scanning {N:,} elements for a value that is not there")
print(f"  (median of {REPEATS} repeats after one discarded warm-up)\n")
a = timed(builtin, "built-in list")
b = timed(handbuilt, "hand-built LinkedList")
print(f"\n  ratio: the hand-built list is {b / a:.1f}x SLOWER")
print("  Both are O(n). Complexity predicted a tie; the constant factor did not.")
