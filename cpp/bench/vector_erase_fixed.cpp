#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> items = {
        "keep", "remove", "keep", "remove", "keep"
    };

    for (auto it = items.begin(); it != items.end();) {
        if (*it == "remove") {
            it = items.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& item : items) {
        std::cout << item << '\n';
    }

    return 0;
}
