#ifndef PINJAMLAB_STRUCTURES_LINKED_LIST_HPP
#define PINJAMLAB_STRUCTURES_LINKED_LIST_HPP

// From-scratch primitive: a singly linked list with an explicit Rule of Five.
// The point of writing this by hand is the pointer surgery in insertAfter()
// and erase() -- the two places where a drawn diagram and the code must agree.

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

namespace pinjamlab::structures {

template <typename T>
class LinkedList {
public:
    struct Node {
        T value;
        Node* next = nullptr;
    };

    LinkedList() = default;
    LinkedList(std::initializer_list<T> init) {
        for (const auto& v : init) pushBack(v);
    }

    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& other) {
        for (const Node* n = other.head_; n; n = n->next) pushBack(n->value);
    }
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) { LinkedList tmp(other); swap(tmp); }
        return *this;
    }
    LinkedList(LinkedList&& other) noexcept { swap(other); }
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) { clear(); swap(other); }
        return *this;
    }

    void swap(LinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    void clear() noexcept {
        while (head_) { Node* next = head_->next; delete head_; head_ = next; }
        tail_ = nullptr;
        size_ = 0;
    }

    void pushFront(const T& v) {
        head_ = new Node{v, head_};
        if (!tail_) tail_ = head_;
        ++size_;
    }

    void pushBack(const T& v) {
        Node* n = new Node{v, nullptr};
        if (tail_) tail_->next = n; else head_ = n;
        tail_ = n;
        ++size_;
    }

    // Insert after the node currently at index `pos`. O(pos) to walk there,
    // then O(1) to relink -- which is the whole trade a linked list offers.
    bool insertAfter(std::size_t pos, const T& v) {
        Node* n = nodeAt(pos);
        if (!n) return false;
        n->next = new Node{v, n->next};
        if (n == tail_) tail_ = n->next;
        ++size_;
        return true;
    }

    bool erase(std::size_t pos) {
        if (!head_ || pos >= size_) return false;
        Node* doomed = nullptr;
        if (pos == 0) {
            doomed = head_;
            head_ = head_->next;
            if (!head_) tail_ = nullptr;
        } else {
            Node* prev = nodeAt(pos - 1);
            doomed = prev->next;
            prev->next = doomed->next;
            if (doomed == tail_) tail_ = prev;
        }
        delete doomed;
        --size_;
        return true;
    }

    [[nodiscard]] const T* at(std::size_t pos) const {
        const Node* n = nodeAt(pos);
        return n ? &n->value : nullptr;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    [[nodiscard]] std::vector<T> toVector() const {
        std::vector<T> out;
        out.reserve(size_);
        for (const Node* n = head_; n; n = n->next) out.push_back(n->value);
        return out;
    }

private:
    [[nodiscard]] Node* nodeAt(std::size_t pos) {
        Node* n = head_;
        for (std::size_t i = 0; n && i < pos; ++i) n = n->next;
        return n;
    }
    [[nodiscard]] const Node* nodeAt(std::size_t pos) const {
        const Node* n = head_;
        for (std::size_t i = 0; n && i < pos; ++i) n = n->next;
        return n;
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

} // namespace pinjamlab::structures

#endif // PINJAMLAB_STRUCTURES_LINKED_LIST_HPP
