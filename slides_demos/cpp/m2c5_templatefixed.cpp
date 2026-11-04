// Week 5 slide demo -- the fix for m2c2_templateerror.cpp, which DOES compile.
//
// The error said std::hash<AssetTag> was "not enabled". Supplying it is the
// whole fix: the same two-part contract Module 1 week 3 graded -- operator==
// decides which keys are the same, std::hash decides which bucket they land
// in, and a map needs both.
//
// Run:  g++ -std=c++20 -Wall -Wextra -Icpp/include
//           slides_demos/cpp/m2c5_templatefixed.cpp -o bin/m2c5 && ./bin/m2c5

#include "pinjamlab/structures/hash_map.hpp"

#include <functional>
#include <iostream>
#include <string>

struct AssetTag {
    std::string value;
    bool operator==(const AssetTag& other) const { return value == other.value; }
};

// The missing half. Consistency rule: a == b MUST imply hash(a) == hash(b),
// so the hash is computed from exactly the member operator== compares.
template <>
struct std::hash<AssetTag> {
    std::size_t operator()(const AssetTag& t) const noexcept {
        return std::hash<std::string>{}(t.value);
    }
};

int main() {
    pinjamlab::structures::HashMap<AssetTag, std::string> catalog;
    catalog.insert(AssetTag{"LP-001"}, "ThinkPad T14");
    catalog.insert(AssetTag{"PR-014"}, "Epson L3250");

    std::cout << "compiled. size=" << catalog.size()
              << "  bucket of LP-001 = " << catalog.bucketOf(AssetTag{"LP-001"}) << "\n";
    std::cout << "LP-001 -> " << catalog.find(AssetTag{"LP-001"}).value_or("(absent)") << "\n";
    std::cout << "XX-999 -> " << catalog.find(AssetTag{"XX-999"}).value_or("(absent)") << "\n";
    return 0;
}
