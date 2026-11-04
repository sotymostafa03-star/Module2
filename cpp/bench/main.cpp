// The four-way benchmark, C++ side: "look up one tag", three implementations,
// three problem sizes.
//
// Obeys the four rules from Module 2 "Measuring Honestly":
//   1. built at -O2 (see run_bench.sh); the sanitizer build is a separate target
//   2. one discarded warm-up, then the MEDIAN of 7 repeats
//   3. an accumulator that is printed, so the optimiser cannot delete the loop
//   4. containers are built OUTSIDE the timed region
//
// Emits CSV on stdout so the numbers land in sample-data/bench-results.csv
// rather than in somebody's memory of what they saw.

#include "pinjamlab/structures/hash_map.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

using namespace pinjamlab::structures;

template <typename Fn>
double medianMicros(Fn&& op, int repeats = 7) {
    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(repeats));
    op();                                        // warm-up; discarded
    for (int r = 0; r < repeats; ++r) {
        const auto t0 = std::chrono::steady_clock::now();
        const volatile auto sink = op();         // volatile: keep the work alive
        (void)sink;
        const auto t1 = std::chrono::steady_clock::now();
        samples.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

static std::string tagFor(int i) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "LP-%06d", i);
    return std::string(buf);
}

int main() {
    std::printf("implementation,n,ns_per_lookup,lookups_per_batch\n");

    // Rule 5, learned the hard way: SIZE THE BATCH TO THE CLOCK.
    //
    // The first version of this harness timed 200 lookups for every
    // implementation and reported microseconds per batch. A hash lookup takes
    // ~10 ns, so 200 of them is ~2 us -- and steady_clock here has about 1 us
    // of resolution, so the results came out as 1.000, 2.000, 3.000: three
    // significant figures of pure quantisation noise.
    //
    // A batch must be long enough that the clock's granularity is a rounding
    // error, which means the fast implementations need a far bigger batch than
    // the slow one. Time different batch sizes, then normalise to ns/lookup.
    constexpr int kLookupsLinear = 200;        // ~26 s per repeat at n = 1e6
    constexpr int kLookupsHash   = 200000;     // ~2 ms per repeat: 2000x the clock tick

    for (const int n : {1, 2, 5, 10, 20, 30, 50, 100, 10000, 1000000}) {
        // ---- setup, OUTSIDE the timed region (rule 4) ----------------------
        std::vector<std::string> flat;
        flat.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) flat.push_back(tagFor(i));

        std::unordered_map<std::string, int> stdmap;
        stdmap.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) stdmap.emplace(flat[static_cast<std::size_t>(i)], i);

        HashMap<std::string, int> mine(static_cast<std::size_t>(n) * 2);
        for (int i = 0; i < n; ++i) mine.insert(flat[static_cast<std::size_t>(i)], i);

        // Probe keys spread across the range, so no implementation gets a
        // lucky cache-resident prefix.
        const auto makeProbes = [&](int count) {
            std::vector<std::string> p;
            p.reserve(static_cast<std::size_t>(count));
            for (int k = 0; k < count; ++k) p.push_back(tagFor((k * 7919) % n));
            return p;
        };
        const int linearLookups = (n <= 50) ? 20000 : kLookupsLinear;
       const std::vector<std::string> probesLinear = makeProbes(linearLookups);
        const std::vector<std::string> probesHash   = makeProbes(kLookupsHash);

        // ---- linear scan over a vector: O(n) per lookup --------------------
        const double linear = medianMicros([&] {
            long long acc = 0;                   // rule 3: accumulate and return
            for (const auto& p : probesLinear)
                for (std::size_t i = 0; i < flat.size(); ++i)
                    if (flat[i] == p) { acc += static_cast<long long>(i); break; }
            return acc;
        });

        // ---- std::unordered_map: O(1) average ------------------------------
        const double stdhash = medianMicros([&] {
            long long acc = 0;
            for (const auto& p : probesHash) {
                const auto it = stdmap.find(p);
                if (it != stdmap.end()) acc += it->second;
            }
            return acc;
        });

        // ---- the hand-built HashMap: O(1) average, different constants ------
        const double myhash = medianMicros([&] {
            long long acc = 0;
            for (const auto& p : probesHash)
                if (const auto v = mine.find(p)) acc += *v;
            return acc;
        });

        const double nsLinear = linear  * 1000.0 / linearLookups;
        const double nsStd    = stdhash * 1000.0 / kLookupsHash;
        const double nsMine   = myhash  * 1000.0 / kLookupsHash;

        std::printf("cpp_vector_linear_scan,%d,%.1f,%d\n", n, nsLinear, linearLookups);
        std::printf("cpp_std_unordered_map,%d,%.1f,%d\n", n, nsStd, kLookupsHash);
        std::printf("cpp_handbuilt_hashmap,%d,%.1f,%d\n", n, nsMine, kLookupsHash);
        std::fflush(stdout);
    }
    return 0;
}
