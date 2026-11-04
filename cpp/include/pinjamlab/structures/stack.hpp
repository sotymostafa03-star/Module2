#ifndef PINJAMLAB_STRUCTURES_STACK_HPP
#define PINJAMLAB_STRUCTURES_STACK_HPP

// From-scratch primitive: LIFO over a std::vector. Deliberately NOT
// std::stack -- the exercise is to see that "a stack" is a policy over a
// buffer, not a data structure of its own.

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace pinjamlab::structures {

template <typename T>
class Stack {
public:
    void push(const T& v) { data_.push_back(v); }

    void pop() {
        if (data_.empty()) throw std::out_of_range("Stack::pop on empty stack");
        data_.pop_back();
    }

    [[nodiscard]] const T& top() const {
        if (data_.empty()) throw std::out_of_range("Stack::top on empty stack");
        return data_.back();
    }

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }

private:
    std::vector<T> data_;
};

} // namespace pinjamlab::structures

#endif // PINJAMLAB_STRUCTURES_STACK_HPP
