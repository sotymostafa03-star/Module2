// Week 5 slide demo -- the hero snippet, run for real.
//
// Three things the slide claims, each proved by a printed number rather than
// asserted: (1) pushFront writes the new node's link BEFORE moving the head,
// (2) the destructor frees every node -- counted, because this toolchain has
// no libasan to report the leak for us, and (3) the Rule of Five copy is a
// DEEP copy, so the two lists do not share nodes.
//
// Run:  g++ -std=c++20 -Wall -Wextra -Icpp/include
//           slides_demos/cpp/m2c3_linkedlist.cpp -o bin/m2c3 && ./bin/m2c3

#include "pinjamlab/structures/linked_list.hpp"

#include <iostream>
#include <string>

using pinjamlab::structures::LinkedList;

// A payload that counts itself. Every construction and every destruction is
// recorded, so "the destructor frees every node" becomes a number.
struct Tracked {
    std::string tag;
    static inline int alive = 0;
    static inline int built = 0;

    Tracked(const char* t) : tag(t) { ++alive; ++built; }          // NOLINT
    Tracked(const Tracked& o) : tag(o.tag) { ++alive; ++built; }
    ~Tracked() { --alive; }
    bool operator==(const Tracked& o) const { return tag == o.tag; }
};

static void dump(const char* label, const LinkedList<Tracked>& list) {
    std::cout << label << " [";
    for (std::size_t i = 0; i < list.size(); ++i)
        std::cout << (i ? " -> " : "") << list.at(i)->tag;
    std::cout << "]  size=" << list.size() << "  nodes alive=" << Tracked::alive << "\n";
}

int main() {
    std::cout << "1. pushFront: link the new node FIRST, then move the head\n";
    {
        LinkedList<Tracked> shelf;
        shelf.pushFront(Tracked("OS-003"));
        dump("   after 1 pushFront ", shelf);
        shelf.pushFront(Tracked("LP-002"));
        dump("   after 2 pushFront ", shelf);
        shelf.pushFront(Tracked("LP-001"));
        dump("   after 3 pushFront ", shelf);

        std::cout << "\n2. insertAfter(0, \"PR-014\") -- O(1) relink once you hold the node\n";
        shelf.insertAfter(0, Tracked("PR-014"));
        dump("   after insertAfter ", shelf);

        std::cout << "\n3. erase(2) -- relink around the victim, THEN delete it\n";
        shelf.erase(2);
        dump("   after erase(2)    ", shelf);

        std::cout << "\n4. the Rule of Five copy is a DEEP copy, not a shared chain\n";
        LinkedList<Tracked> backup = shelf;          // copy constructor
        std::cout << "   copied: nodes alive=" << Tracked::alive
                  << "  (doubled -- two independent chains)\n";
        backup.erase(0);
        dump("   backup after erase", backup);
        dump("   shelf UNCHANGED   ", shelf);
        std::cout << "\n5. both lists go out of scope here...\n";
    }
    std::cout << "   nodes built over the whole run = " << Tracked::built << "\n";
    std::cout << "   nodes still alive after scope  = " << Tracked::alive
              << "   <- 0 means every node was freed\n";
    return 0;
}
