#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> items = {
        "keep", "remove", "keep", "remove", "keep"
    };

    std::erase_if(items, [](const std::string& item) {
        return item == "remove";
    });

    for (const auto& item : items) {
        std::cout << item << '\n';
    }

    return 0;
}
