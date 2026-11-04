import time
import statistics

SIZES = (100, 10_000, 1_000_000)

def slow_dedupe(matches):
    unique = []
    for item in matches:
        duplicate = False
        for kept in unique:
            if kept == item:
                duplicate = True
                break
        if not duplicate:
            unique.append(item)
    return unique

def fast_dedupe(matches):
    seen = set()
    unique = []
    for item in matches:
        if item not in seen:
            seen.add(item)
            unique.append(item)
    return unique

def measure(function, data):
    samples = []
    for _ in range(3):
        start = time.perf_counter()
        function(data)
        samples.append(time.perf_counter() - start)
    return statistics.median(samples)

print("implementation,n,time_seconds")

for n in SIZES:
    data = [f"LP-{i:07d}" for i in range(n)]

    if n <= 10_000:
        elapsed = measure(slow_dedupe, data)
        print(f"nested_loop,{n},{elapsed:.6f}")
    else:
        print(f"nested_loop,{n},NOT_COMPLETED")

    elapsed = measure(fast_dedupe, data)
    print(f"set_based,{n},{elapsed:.6f}")
