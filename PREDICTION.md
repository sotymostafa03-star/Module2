# Lab 3.1 — Benchmark Predictions

## Expected Performance

The benchmark compares five implementations:

1. C++ vector linear scan
2. C++ std::unordered_map
3. C++ custom HashMap
4. Python list linear scan
5. Python dict

The input sizes are 100, 10,000, and 1,000,000 records.

## Predictions

| Implementation | n = 100 | n = 10,000 | n = 1,000,000 |
|---|---|---|---|
| C++ vector linear scan | Slow | Much slower | Slowest |
| C++ std::unordered_map | Very fast | Very fast | Very fast |
| C++ custom HashMap | Very fast | Very fast | Very fast |
| Python list linear scan | Slow | Much slower | Slowest |
| Python dict | Very fast | Very fast | Very fast |

## Expected Lookup Complexity

| Implementation | Expected Complexity |
|---|---|
| C++ vector linear scan | O(n) |
| C++ std::unordered_map | Average O(1) |
| C++ custom HashMap | Average O(1) |
| Python list linear scan | O(n) |
| Python dict | Average O(1) |

## Expected Ranking

For large datasets, hash-based lookup implementations are expected to outperform linear scans.

C++ hash-based implementations are expected to be faster than Python dict because C++ is compiled, although actual results depend on implementation and hardware.

The custom C++ HashMap may be slower or faster than std::unordered_map depending on hashing, collisions, memory layout, and implementation overhead.

For small datasets, linear scans may remain competitive because they have low overhead and good cache locality.

## Measurement Plan

- Use the fixed sample catalogue and the personal catalogue where applicable.
- Build C++ benchmark code with -O2.
- Build containers outside the timed region.
- Discard the first warm-up run.
- Use the median of seven repetitions.
- Accumulate and print lookup results.
- Report nanoseconds per lookup.
- Test input sizes of 100, 10,000, and 1,000,000.

## Benchmark Timing Note

The benchmark was executed before this prediction file was created. Therefore, this file records the expected behavior retrospectively and does not establish a prediction committed before measurement.
