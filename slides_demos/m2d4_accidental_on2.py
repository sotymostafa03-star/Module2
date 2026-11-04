"""Week 6 slide demo -- the accidental O(n^2), measured, then indexed.

The read path is `by_category(cat)`: a linear scan of every loaded row. That is
O(n) PER CALL, and it is the correct complexity for one query. The defect is
the CALLER in the report loop: it asks for every category, once per row, so the
catalogue is walked rows x categories times.

The module's own service carries both shapes -- the scan in
`py/pinjamlab/catalog_service.py`, the index in
`cpp/include/pinjamlab/catalog_service.hpp` -- and this program runs the same
loop shape so the deficit is a measurement rather than an assertion.

Run:  python slides_demos/m2d4_accidental_on2.py
"""
import time

CATEGORIES = 40


def build_units(n):
    """n rows spread over CATEGORIES categories -- a realistic catalogue."""
    return [("T-%06d" % i, "CAT-%02d" % (i % CATEGORIES)) for i in range(n)]


def scan_by_category(units, cat):
    """O(n) per call, and correct. This is the catalogue's existing method."""
    return [u for u in units if u[1] == cat]


def run_scan(units):
    """The DEFECT: one scan per row, so the catalogue is walked rows x cats."""
    hits = 0
    for _ in units:
        for c in range(CATEGORIES):
            hits += len(scan_by_category(units, "CAT-%02d" % c))
    return hits


def run_indexed(units):
    """Group once, O(n); every category query is then a dict lookup."""
    groups, hits = {}, 0
    for t, c in units:
        groups.setdefault(c, []).append(t)
    for _ in units:
        for c in range(CATEGORIES):
            hits += len(groups.get("CAT-%02d" % c, ()))
    return hits


def main():
    times = []
    print("%10s %12s %12s %10s %10s" %
          ("rows", "scan (ms)", "indexed (ms)", "ratio", "same?"))
    for n in (500, 2000):
        units = build_units(n)
        t0 = time.perf_counter(); hits_s = run_scan(units); t1 = time.perf_counter()
        t2 = time.perf_counter(); hits_i = run_indexed(units); t3 = time.perf_counter()
        times.append((t1 - t0, t3 - t2))
        print("%10d %12.1f %12.1f %10s %10s" %
              (n, (t1 - t0) * 1e3, (t3 - t2) * 1e3,
               "%.1fx" % ((t1 - t0) / (t3 - t2)),
               "yes" if hits_s == hits_i else "NO"))
    (s1, i1), (s2, i2) = times
    print("\nrows x4  ->  scan x%.1f,  index x%.1f" % (s2 / s1, i2 / i1))
    print("rows scanned per category query: %d for the scan, 1 for the index"
          % len(units))
    print("Both columns above returned the same answer. Only the time differs.")


if __name__ == "__main__":
    main()
