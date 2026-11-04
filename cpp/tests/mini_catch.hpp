#ifndef MINI_CATCH_HPP
#define MINI_CATCH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cstdlib>

namespace test_runner {

struct TestUnit {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestUnit>& registry() {
    static std::vector<TestUnit> reg;
    return reg;
}

struct Registerer {
    Registerer(const std::string& name, std::function<void()> f) {
        registry().push_back({name, std::move(f)});
    }
};

inline int runAll() {
    int passed = 0;
    int failed = 0;
    std::cout << "===============================================================================\n";
    std::cout << "Running test suite (" << registry().size() << " test cases)...\n";
    std::cout << "===============================================================================\n";

    for (const auto& t : registry()) {
        try {
            t.func();
            std::cout << "PASSED: " << t.name << "\n";
            ++passed;
        } catch (const std::exception& ex) {
            std::cout << "FAILED: " << t.name << " (Exception: " << ex.what() << ")\n";
            ++failed;
        } catch (...) {
            std::cout << "FAILED: " << t.name << " (Unknown exception)\n";
            ++failed;
        }
    }

    std::cout << "===============================================================================\n";
    std::cout << "All tests finished: " << passed << " passed, " << failed << " failed.\n";
    std::cout << "===============================================================================\n";
    return (failed == 0) ? 0 : 1;
}

} // namespace test_runner

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define TEST_CASE(name) \
    static void CONCAT(test_body_, __LINE__)(); \
    static ::test_runner::Registerer CONCAT(reg_, __LINE__)(name, CONCAT(test_body_, __LINE__)); \
    static void CONCAT(test_body_, __LINE__)()

#define REQUIRE(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Assertion failed: " #expr " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed: " #expr); \
        } \
    } while (false)

#define REQUIRE_THROWS_AS(expr, exc_type) \
    do { \
        bool caught = false; \
        try { \
            (expr); \
        } catch (const exc_type&) { \
            caught = true; \
        } catch (...) {} \
        if (!caught) { \
            std::cerr << "Expected exception " #exc_type " not thrown at " << __FILE__ << ":" << __LINE__ << "\n"; \
            throw std::runtime_error("Expected " #exc_type " was not thrown."); \
        } \
    } while (false)

#endif // MINI_CATCH_HPP
