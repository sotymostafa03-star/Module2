// Week 5 slide demo -- Pitfall C4 made into a measurement.
//
// Two hash functions, the same 20 keys from sample-catalog.csv, the same
// bucket count. std::hash spreads them; a hash that returns the key's LENGTH
// puts every tag in one bucket, because every PinjamLab tag is 6 characters.
// Correctness is identical in both cases -- every lookup still returns the
// right answer, which is exactly why no test catches this.
//
// Run:  g++ -std=c++20 -Wall -Wextra -Icpp/include
//           slides_demos/cpp/m2c4_buckets.cpp -o bin/m2c4 && ./bin/m2c4

#include "pinjamlab/structures/hash_map.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

using pinjamlab::structures::HashMap;

// The defect from Pitfall C4: legal, deterministic, and useless as a hash.
struct LengthHash {
    std::size_t operator()(const std::string& s) const { return s.size(); }
};

static const std::vector<std::string> kTags = {
    "LP-001", "LP-002", "LP-003", "LP-004", "PR-014", "PR-015", "OS-003",
    "OS-004", "MC-007", "MC-008", "RA-001", "RA-002", "SC-011", "SC-012",
    "TB-021", "TB-022", "MB-031", "MB-032", "CM-041", "CM-042",
};

template <typename Map>
static void report(const char* label, Map& map) {
    for (const auto& tag : kTags) map.insert(tag, "x");

    std::cout << label << "\n";
    std::cout << "  buckets=" << map.bucketCount()
              << "  size=" << map.size()
              << "  load=" << map.loadFactor()
              << "  rehashes=" << map.rehashCount() << "\n  chain lengths: ";

    std::size_t longest = 0, occupied = 0, comparisons = 0;
    for (std::size_t b = 0; b < map.bucketCount(); ++b) {
        const std::size_t len = map.bucketSize(b);
        std::cout << len << (b + 1 < map.bucketCount() ? " " : "");
        if (len > longest) longest = len;
        if (len > 0) ++occupied;
        comparisons += len * (len + 1) / 2;   // cost of finding each key in it
    }
    // Average comparisons to find a key that IS present.
    const double avg = static_cast<double>(comparisons) / static_cast<double>(kTags.size());
    std::cout << "\n  longest chain=" << longest
              << "  buckets used=" << occupied << "/" << map.bucketCount()
              << "  avg comparisons per successful lookup=" << avg << "\n\n";
}

int main() {
    std::cout << "20 asset tags, every one 6 characters long.\n\n";

    HashMap<std::string, std::string> good;
    report("std::hash<std::string>  -- spreads", good);

    HashMap<std::string, std::string, LengthHash> bad;
    report("LengthHash (Pitfall C4) -- does not", bad);

    std::cout << "Both maps return the correct value for all 20 keys.\n";
    HashMap<std::string, std::string, LengthHash> check;
    for (const auto& tag : kTags) check.insert(tag, tag + "-model");
    std::size_t ok = 0;
    for (const auto& tag : kTags)
        if (check.find(tag) == tag + "-model") ++ok;
    std::cout << "LengthHash lookups correct: " << ok << "/" << kTags.size()
              << "  <- the test suite stays green while the map degrades to a list.\n";
    return 0;
}
