#include <chrono>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using Clock = std::chrono::steady_clock;

std::vector<std::string> slowDedupe(
    const std::vector<std::string>& matches
) {
    std::vector<std::string> unique;

    for (const auto& item : matches) {
        bool duplicate = false;

        for (const auto& kept : unique) {
            if (kept == item) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            unique.push_back(item);
        }
    }

    return unique;
}

std::vector<std::string> fastDedupe(
    const std::vector<std::string>& matches
) {
    std::unordered_set<std::string> seen;
    std::vector<std::string> unique;

    for (const auto& item : matches) {
        if (seen.insert(item).second) {
            unique.push_back(item);
        }
    }

    return unique;
}

template <typename Function>
double measure(Function function,
               const std::vector<std::string>& data) {
    const auto start = Clock::now();
    const auto result = function(data);
    const auto end = Clock::now();

    if (result.size() != data.size()) {
        std::cerr << "Unexpected result size\n";
    }

    return std::chrono::duration<double>(end - start).count();
}

int main() {
    const std::vector<std::size_t> sizes = {
        100, 10'000, 1'000'000
    };

    std::cout << "implementation,n,time_seconds\n";

    for (const auto n : sizes) {
        std::vector<std::string> data;
        data.reserve(n);

        for (std::size_t i = 0; i < n; ++i) {
            data.push_back("LP-" + std::to_string(i));
        }

        if (n <= 10'000) {
            std::cout << "nested_loop," << n << ","
                      << measure(slowDedupe, data) << '\n';
        } else {
            std::cout << "nested_loop," << n
                      << ",NOT_COMPLETED\n";
        }

        std::cout << "unordered_set," << n << ","
                  << measure(fastDedupe, data) << '\n';
    }

    return 0;
}
