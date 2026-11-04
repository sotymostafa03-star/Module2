# PinjamLab Module 2 — Data Structures, STL Containers & Templates

Part of **Laboratorium Pemrograman II** (`25MP7716G014`), S1 Informatika,
Universitas Muhammadiyah Surabaya. Extends the same repository as Module 1.

## Unit verification table

| Unit / test file | Language | Concept verified | Outcome |
|---|---|---|---|
| `cpp/tests/test_structures.cpp` | C++20 | Stack LIFO, Queue ring-buffer FIFO + growth, LinkedList pointer surgery, Rule of Five deep copy, HashMap vs `std::unordered_map`, rehash | **6 passed** |
| `cpp/tests/test_catalog.cpp` | C++20 | 20-row load, duplicate-tag rejection, case-insensitive `search()`, category partition, index-backed `contains()` | **5 passed** |
| `py/tests/test_structures.py` | Python 3.12 | LinkedList vs `list`, erase/tail fix-up, membership, Repository duplicate rule, and the same five catalogue behaviours | **17 passed** |

## One command

```bash
./run_tests.sh      # build (-Wall -Wextra -Werror), C++ suites, pytest
./run_bench.sh      # the four-way benchmark at -O2 -> sample-data/bench-results.csv
```

`run_tests.sh` **exits non-zero when a toolchain is missing.** Module 1's
script printed its `[SUCCESS]` banner even when it found no compiler and
skipped the entire C++ half; that made a green banner worthless as evidence.
This one refuses instead.

## The measured benchmark

`sample-data/bench-results.csv` is committed. Nanoseconds per lookup, median of
7 repeats after a discarded warm-up, containers built outside the timed region,
results accumulated into a printed value so the optimiser cannot delete the
work.

| Implementation | n = 10² | n = 10⁴ | n = 10⁶ |
|---|---:|---:|---:|
| C++ `vector` linear scan | 165.0 | 14 110.0 | 1 410 350.0 |
| C++ `std::unordered_map` | 7.8 | 21.8 | 134.5 |
| C++ hand-built `HashMap` | 12.1 | 17.0 | 115.2 |
| Python `list` linear scan | 1 002.5 | 115 135.5 | 11 315 050.5 |
| Python `dict` | 37.8 | 60.1 | 655.9 |

Three things in that table are worth arguing about in the viva, and none of
them is predictable from complexity alone:

1. **The hash rows are not flat.** They rise about 17× from n = 10² to 10⁶.
   Big-O says a hash lookup is O(1); the cache says a lookup that misses L2
   costs more than one that does not. Both are true.
2. **The hand-built map beats `std::unordered_map` at n = 10⁶** (115 ns vs
   135 ns) and loses to it at n = 10². Be suspicious of the win: this
   benchmark only ever looks up keys that are present, and a hand-built map
   with `bucketCount = 2n` never rehashes mid-run. Neither is true of real use.
3. **The spread at n = 10⁶ is about 98 000×** between the slowest (Python
   linear scan, 11.3 ms) and the fastest (C++ hand-built map, 115 ns). That is
   the number the GTA Online story is really about.

### Sizing the batch to the clock

The first version of the harness timed 200 lookups for every implementation and
reported microseconds. A hash lookup is ~10 ns, so the batch took ~2 µs against
a `steady_clock` with ~1 µs resolution, and the results came out as `1.000`,
`2.000`, `3.000` — three significant figures of quantisation noise. The harness
now uses 200 lookups for the linear scan and 200 000 for the hash maps, then
normalises to ns per lookup. **A benchmark whose numbers are suspiciously round
is measuring its clock, not your code.**

## Verified

- **Environment:** Windows 11 · g++ (GCC) 15.2.0 · Python 3.12.13 · pytest 8.3.4
- **Date:** 2026-09-15
- **Result:** 11 C++ test cases + 17 Python tests pass; no warnings under
  `-Wall -Wextra -Werror`; benchmark run at `-O2`.

## Directory structure

```
Module2/
├── run_tests.sh            # build + both suites; fails if a toolchain is absent
├── run_bench.sh            # the four-way benchmark, -O2, CSV out
├── cpp/
│   ├── include/pinjamlab/
│   │   ├── structures/     # the from-scratch primitives
│   │   │   ├── stack.hpp       # LIFO over a vector
│   │   │   ├── queue.hpp       # FIFO as a ring buffer
│   │   │   ├── linked_list.hpp # singly linked; Rule of Five
│   │   │   └── hash_map.hpp    # bucket array, chaining, rehash at load 0.75
│   │   ├── repository.hpp      # generic Repository<T, ID>
│   │   └── catalog_service.hpp # search() -- the Lab 2 diagnose-and-fix target
│   ├── bench/main.cpp
│   └── tests/
├── py/
│   ├── pinjamlab/
│   │   ├── structures/{linked_list,repository}.py
│   │   └── catalog_service.py
│   ├── bench/bench_lookup.py
│   └── tests/
├── slides_demos/           # the transcripts behind slides/Module2_Week*.html
└── sample-data/
    ├── sample-catalog.csv  # fixed, committed, 20 rows, shared by BOTH trees
    └── bench-results.csv   # measured; regenerate with ./run_bench.sh
```

`binary_tree.hpp` and `my-catalog.csv` are **deliberately absent**: the category
tree is week 5's from-scratch primitive, and `my-catalog.csv` (≥ 50 rows) is the
bring-your-own-input each student supplies in Lab 1.
