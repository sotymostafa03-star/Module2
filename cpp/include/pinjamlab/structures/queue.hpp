#ifndef PINJAMLAB_STRUCTURES_QUEUE_HPP
#define PINJAMLAB_STRUCTURES_QUEUE_HPP

// From-scratch primitive: FIFO as a ring buffer over a fixed-capacity vector.
// A ring is used rather than "erase the front of a vector" precisely because
// erasing the front is O(n) -- and that difference is the lesson.

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace pinjamlab::structures {

template <typename T>
class Queue {
public:
    explicit Queue(std::size_t capacity = 16)
        : buf_(capacity == 0 ? 1 : capacity) {}

    void push(const T& v) {
        if (size_ == buf_.size()) grow();
        buf_[(head_ + size_) % buf_.size()] = v;
        ++size_;
    }

    void pop() {
        if (size_ == 0) throw std::out_of_range("Queue::pop on empty queue");
        head_ = (head_ + 1) % buf_.size();
        --size_;
    }

    [[nodiscard]] const T& front() const {
        if (size_ == 0) throw std::out_of_range("Queue::front on empty queue");
        return buf_[head_];
    }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return buf_.size(); }

private:
    void grow() {
        std::vector<T> bigger(buf_.size() * 2);
        for (std::size_t i = 0; i < size_; ++i)
            bigger[i] = buf_[(head_ + i) % buf_.size()];
        buf_.swap(bigger);
        head_ = 0;
    }

    std::vector<T> buf_;
    std::size_t head_ = 0;
    std::size_t size_ = 0;
};

} // namespace pinjamlab::structures

#endif // PINJAMLAB_STRUCTURES_QUEUE_HPP
