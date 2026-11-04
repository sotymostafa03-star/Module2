// Week 4 slide demo — iterator invalidation, with the real addresses.
//
// Module 2 Figure 9 draws this with illustrative addresses. This prints the
// ones your machine actually used, which is the point: the buffer MOVES, and
// the iterator you were holding still points at the old place.
//
// Build & run (from code/Module2):
//   g++ -std=c++20 -Wall -Wextra slides_demos/cpp/m2c1_iterinvalid.cpp -o bin/m2c1
//   ./bin/m2c1

#include <cstdio>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> shelf{"LP-001", "PR-014", "LP-002", "OS-003"};
    shelf.reserve(5);                       // capacity 5, size 4

    const auto* before = shelf.data();
    auto it = shelf.begin() + 2;            // points at LP-002

    std::printf("BEFORE  buffer=%p  size=%zu  capacity=%zu  *it=%s\n",
                static_cast<const void*>(before), shelf.size(), shelf.capacity(),
                it->c_str());

    shelf.push_back("MC-007");              // size 5 == capacity: still fine
    std::printf("push 5  buffer=%p  size=%zu  capacity=%zu  (no reallocation)\n",
                static_cast<const void*>(shelf.data()), shelf.size(), shelf.capacity());

    shelf.push_back("RA-001");              // size 6 > capacity 5: REALLOCATES
    const auto* after = shelf.data();
    std::printf("push 6  buffer=%p  size=%zu  capacity=%zu  <- buffer MOVED\n",
                static_cast<const void*>(after), shelf.size(), shelf.capacity());

    std::printf("\nthe buffer moved by %lld bytes\n",
                static_cast<long long>(after - before) *
                    static_cast<long long>(sizeof(std::string)));
    std::printf("`it` still holds the OLD address. Dereferencing it now is\n"
                "undefined behaviour: the block is freed, and *it usually\n"
                "still reads \"LP-002\" because nothing has reused it yet.\n");
    std::printf("\nThat is why the test passes and the bug ships.\n");
    std::printf("The safe answer is an INDEX, which survives reallocation:\n");
    std::printf("  shelf[2] = %s\n", shelf[2].c_str());
    return 0;
}
