// Module 2 unit tests: the from-scratch primitives must agree with the
// standard containers they are modelled on. "It compiles" is not the bar --
// the bar is that swapping mine for the library's changes no observable
// behaviour.

#include "mini_catch.hpp"
#include "pinjamlab/structures/hash_map.hpp"
#include "pinjamlab/structures/linked_list.hpp"
#include "pinjamlab/structures/queue.hpp"
#include "pinjamlab/structures/stack.hpp"

#include <string>
#include <unordered_map>
#include <vector>

using namespace pinjamlab::structures;

TEST_CASE("Stack: LIFO order and empty-stack guards") {
    Stack<int> s;
    REQUIRE(s.empty());
    for (int i = 1; i <= 4; ++i) s.push(i);
    REQUIRE(s.size() == 4);
    REQUIRE(s.top() == 4);
    s.pop();
    REQUIRE(s.top() == 3);
    while (!s.empty()) s.pop();
    REQUIRE_THROWS_AS(s.pop(), std::out_of_range);
    REQUIRE_THROWS_AS((void)s.top(), std::out_of_range);
}

TEST_CASE("Queue: FIFO order survives wrap-around and growth") {
    Queue<int> q(2);                 // deliberately tiny, to force a grow
    q.push(1); q.push(2);
    REQUIRE(q.front() == 1);
    q.pop();
    q.push(3); q.push(4);            // wraps, then grows
    REQUIRE(q.size() == 3);
    std::vector<int> drained;
    while (!q.empty()) { drained.push_back(q.front()); q.pop(); }
    REQUIRE((drained == std::vector<int>{2, 3, 4}));
    REQUIRE_THROWS_AS(q.pop(), std::out_of_range);
}

TEST_CASE("LinkedList: pointer surgery keeps order, size and tail correct") {
    LinkedList<std::string> l{"a", "c"};
    l.insertAfter(0, "b");                       // a -> b -> c
    REQUIRE((l.toVector() == std::vector<std::string>{"a", "b", "c"}));
    l.pushFront("start");
    l.pushBack("end");
    REQUIRE(l.size() == 5);
    REQUIRE(*l.at(0) == "start");
    REQUIRE(*l.at(4) == "end");

    l.erase(0);                                   // erase the head
    REQUIRE(*l.at(0) == "a");
    l.erase(l.size() - 1);                        // erase the tail
    REQUIRE((l.toVector() == std::vector<std::string>{"a", "b", "c"}));
    l.pushBack("tail-still-works");                // proves tail_ was fixed up
    REQUIRE(*l.at(3) == "tail-still-works");
    REQUIRE(l.erase(99) == false);
}

TEST_CASE("LinkedList: Rule of Five gives a deep copy, not a shared one") {
    LinkedList<int> original{1, 2, 3};
    LinkedList<int> copy = original;              // copy ctor
    copy.pushBack(4);
    REQUIRE(original.size() == 3);                // untouched
    REQUIRE(copy.size() == 4);

    LinkedList<int> assigned{9};
    assigned = original;                          // copy assignment
    REQUIRE((assigned.toVector() == std::vector<int>{1, 2, 3}));

    LinkedList<int> moved = std::move(copy);      // move ctor
    REQUIRE(moved.size() == 4);
}

TEST_CASE("HashMap: matches std::unordered_map on the same key sequence") {
    HashMap<std::string, int> mine;
    std::unordered_map<std::string, int> theirs;

    const std::vector<std::string> keys = {
        "LP-001", "PR-014", "LP-002", "OS-003", "MC-007", "LP-001"
    };
    for (std::size_t i = 0; i < keys.size(); ++i) {
        mine.insert(keys[i], static_cast<int>(i));
        theirs[keys[i]] = static_cast<int>(i);
    }

    REQUIRE(mine.size() == theirs.size());        // LP-001 inserted twice -> 5
    REQUIRE(mine.size() == 5);
    for (const auto& [k, v] : theirs) {
        const auto got = mine.find(k);
        REQUIRE(got.has_value());
        REQUIRE(*got == v);
    }
    REQUIRE(!mine.contains("NOT-THERE"));
    REQUIRE(mine.erase("LP-002"));
    REQUIRE(!mine.contains("LP-002"));
    REQUIRE(mine.size() == 4);
    REQUIRE(!mine.erase("LP-002"));               // erasing twice is not a crash
}

TEST_CASE("HashMap: rehash preserves every entry and lowers the load factor") {
    HashMap<int, int> m(4);
    for (int i = 0; i < 100; ++i) m.insert(i, i * i);

    REQUIRE(m.size() == 100);
    REQUIRE(m.bucketCount() > 4);                 // it grew
    REQUIRE(m.rehashCount() > 0);
    REQUIRE(m.loadFactor() <= 0.75);              // the invariant that forces it
    for (int i = 0; i < 100; ++i) {
        const auto got = m.find(i);
        REQUIRE(got.has_value());
        REQUIRE(*got == i * i);
    }
}

int main() { return test_runner::runAll(); }
