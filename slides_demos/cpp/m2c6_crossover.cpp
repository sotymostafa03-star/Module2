// Week 6 slide demo -- Lab 3.4: where does the linear scan actually WIN?
//
// The whole module argues for hash maps. This program finds the n below which
// that argument is wrong. Same harness discipline the module demands of
// students (Listing lst:bench): -O2, warm-up discarded, median of repeats,
// results accumulated so the optimiser cannot delete the loop, batch sized to
// the clock rather than to a round number.
//
// Methodology note, learned the hard way: the first version of this harness
// computed the lookup key INSIDE the timed loop (`vec[b % n]`). The integer
// modulo and the string reference then sat in both timed regions and dominated
// them, which made a hash lookup appear to grow with n -- a measurement of the
// harness, not of the map. The keys are now precomputed into `probes` before
// the clock starts, so each timed region contains only the lookup itself.
//
// Run:  g++ -std=c++20 -O2 -Wall -Wextra slides_demos/cpp/m2c6_crossover.cpp
//           -o bin/m2c6 && ./bin/m2c6

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

using Clock = std::chrono::steady_clock;

static std::string tagOf(int i) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "LP-%03d", i);
    return std::string(buf);
}

int main() {
    const int kRepeats = 9;
    const long kBatch = 100000;
    std::printf("%6s %14s %14s %10s\n", "n", "vector (ns)", "hashmap (ns)", "winner");

    int crossover = -1;
    for (int n : {1, 2, 3, 4, 5, 6, 8, 10, 12, 16, 24, 32, 64}) {
        std::vector<std::string> vec;
        std::unordered_map<std::string, int> map;
        map.reserve(static_cast<std::size_t>(n) * 2);
        for (int i = 0; i < n; ++i) { vec.push_back(tagOf(i)); map.emplace(tagOf(i), i); }

        // Every probe key resolved BEFORE the clock starts: the timed regions
        // below contain the lookup and nothing else.
        std::vector<const std::string*> probes;
        probes.reserve(static_cast<std::size_t>(kBatch));
        for (long b = 0; b < kBatch; ++b)
            probes.push_back(&vec[static_cast<std::size_t>(b % n)]);

        std::vector<double> vs, hs;
        for (int r = 0; r <= kRepeats; ++r) {          // r == 0 is the warm-up
            long sink = 0;
            auto t0 = Clock::now();
            for (const std::string* key : probes)
                for (const auto& v : vec) if (v == *key) { ++sink; break; }
            auto t1 = Clock::now();
            for (const std::string* key : probes)
                if (map.find(*key) != map.end()) ++sink;
            auto t2 = Clock::now();
            if (sink == 0) std::printf("!");           // keep `sink` observable
            if (r == 0) continue;                      // discard the warm-up
            vs.push_back(std::chrono::duration<double, std::nano>(t1 - t0).count() / kBatch);
            hs.push_back(std::chrono::duration<double, std::nano>(t2 - t1).count() / kBatch);
        }
        std::sort(vs.begin(), vs.end());
        std::sort(hs.begin(), hs.end());
        const double v = vs[vs.size() / 2], h = hs[hs.size() / 2];
        const bool vecWins = v < h;
        if (!vecWins && crossover < 0) crossover = n;
        std::printf("%6d %14.1f %14.1f %10s\n", n, v, h, vecWins ? "vector" : "hashmap");
    }
    std::printf("\nThe hash map first wins at n = %d.\n", crossover);
    return 0;
}
