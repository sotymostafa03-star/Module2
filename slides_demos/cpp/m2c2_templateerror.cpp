// Week 5 slide demo -- DELIBERATELY DOES NOT COMPILE.
//
// The error message IS the artifact. `AssetTag` is a perfectly good value
// type: it has operator==, so it works in a LinkedList<AssetTag> all day.
// What it does NOT have is a std::hash specialisation -- the other half of
// the contract Module 1 week 3 graded. HashMap<K,V> defaults its Hash
// parameter to std::hash<K>, so the missing half only surfaces here, at the
// point of INSTANTIATION, not at the point of definition.
//
// Capture with:  g++ -std=c++20 -Icpp/include slides_demos/cpp/m2c2_templateerror.cpp 2>&1

#include "pinjamlab/structures/hash_map.hpp"

#include <string>

struct AssetTag {
    std::string value;
    bool operator==(const AssetTag& other) const { return value == other.value; }
};

int main() {
    pinjamlab::structures::HashMap<AssetTag, std::string> catalog;
    catalog.insert(AssetTag{"LP-001"}, "ThinkPad T14");
    return 0;
}
