from __future__ import annotations

import statistics
import time
from pathlib import Path
import sys

PROJECT_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(PROJECT_ROOT / "py"))

from pinjamlab.structures.linked_list import LinkedList


def median_time_per_lookup(container, target, repeats=7, lookups=100):
    def run():
        for _ in range(lookups):
            target in container

    run()  # Warm-up

    samples = []

    for _ in range(repeats):
        start = time.perf_counter()

        run()

        elapsed = time.perf_counter() - start
        samples.append(elapsed / lookups)

    return statistics.median(samples)


def main():
    n = 10_000

    values = list(range(n))
    target = -1

    builtin_list = values
    linked_list = LinkedList(values)

    list_time = median_time_per_lookup(
        builtin_list,
        target
    )

    linked_time = median_time_per_lookup(
        linked_list,
        target
    )

    ratio = linked_time / list_time

    print("Implementation,n,median_ns_per_lookup")
    print(f"Python_list,{n},{list_time * 1e9:.2f}")
    print(f"Hand_built_LinkedList,{n},{linked_time * 1e9:.2f}")
    print()
    print(f"LinkedList_vs_list_ratio: {ratio:.2f}x")
    print(f"Target: {target} (not present)")
    print(f"Repeats: 7")
    print(f"Lookups per repeat: 100")


if __name__ == "__main__":
    main()