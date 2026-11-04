"""The four-way benchmark, Python side: `dict` and a linear scan over `list`.

Same discipline as the C++ harness (Module 2, "Measuring Honestly"):
warm-up discarded, median of repeats, setup outside the timed region, and a
batch sized so the clock's granularity is a rounding error.

Run:  python py/bench/bench_lookup.py  >  sample-data/bench-py.csv
"""
from __future__ import annotations

import timeit

SIZES = (1, 2, 5, 10, 20, 30, 50, 100, 10_000, 1_000_000)
LOOKUPS_LINEAR = 200          # default linear lookup batch
LOOKUPS_HASH = 200_000        # a dict lookup is ~50 ns; make the batch long


def tag_for(i: int) -> str:
    return f"LP-{i:06d}"


def median_ns_per_lookup(stmt: str, setup: str, lookups: int,
                         repeats: int = 7) -> float:
    # timeit disables the GC during the run, which is what we want: we are
    # measuring the lookup, not a collection that happens to fire.
    samples = timeit.repeat(stmt, setup=setup, repeat=repeats, number=1)
    samples.sort()
    median_seconds = samples[len(samples) // 2]
    return median_seconds / lookups * 1e9


def main() -> None:
    print("implementation,n,ns_per_lookup,lookups_per_batch")
    for n in SIZES:
        common = (
            "from __main__ import tag_for\n"
            f"n = {n}\n"
            "flat = [tag_for(i) for i in range(n)]\n"
            "table = {t: i for i, t in enumerate(flat)}\n"
        )

        linear = median_ns_per_lookup(
            stmt=(
                "acc = 0\n"
                "for p in probes:\n"
                "    for i, v in enumerate(flat):\n"
                "        if v == p:\n"
                "            acc += i\n"
                "            break\n"
            ),
            setup=common + f"probes = [tag_for((k * 7919) % n) for k in range({20000 if n <= 50 else LOOKUPS_LINEAR})]\n",
            lookups=(20000 if n <= 50 else LOOKUPS_LINEAR),
        )

        hashed = median_ns_per_lookup(
            stmt=(
                "acc = 0\n"
                "for p in probes:\n"
                "    acc += table[p]\n"
            ),
            setup=common + f"probes = [tag_for((k * 7919) % n) for k in range({LOOKUPS_HASH})]\n",
            lookups=LOOKUPS_HASH,
        )

        print(f"py_list_linear_scan,{n},{linear:.1f},{20000 if n <= 50 else LOOKUPS_LINEAR}")
        print(f"py_dict,{n},{hashed:.1f},{LOOKUPS_HASH}")


if __name__ == "__main__":
    main()
