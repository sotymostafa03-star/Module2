#ifndef PINJAMLAB_STRUCTURES_HASH_MAP_HPP
#define PINJAMLAB_STRUCTURES_HASH_MAP_HPP

// From-scratch primitive (MASTER 3.2 item 2): a separately-chained hash map.
// Built here rather than used from <unordered_map> because building one is
// what makes the load factor, the bucket array and the rehash visible.

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace pinjamlab::structures {

template <typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
public:
    // A bucket is a chain of nodes. Chaining, not open addressing, because a
    // chain is something a student can draw on the whiteboard in the viva.
    struct Node {
        K key;
        V value;
        Node* next = nullptr;
    };

    explicit HashMap(std::size_t initialBuckets = 8)
        : buckets_(initialBuckets == 0 ? 1 : initialBuckets, nullptr) {}

    ~HashMap() { clear(); }

    // Rule of Five: this type owns raw nodes, so copying it must be explicit.
    HashMap(const HashMap& other) : buckets_(other.buckets_.size(), nullptr) {
        for (const auto* head : other.buckets_)
            for (const auto* n = head; n; n = n->next) insert(n->key, n->value);
    }
    HashMap& operator=(const HashMap& other) {
        if (this != &other) { HashMap tmp(other); swap(tmp); }
        return *this;
    }
    HashMap(HashMap&& other) noexcept { swap(other); }
    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) { clear(); swap(other); }
        return *this;
    }

    void swap(HashMap& other) noexcept {
        buckets_.swap(other.buckets_);
        std::swap(size_, other.size_);
        std::swap(rehashes_, other.rehashes_);
    }

    void clear() noexcept {
        for (auto*& head : buckets_) {
            while (head) { Node* next = head->next; delete head; head = next; }
        }
        size_ = 0;
    }

    // Returns true if the key was newly inserted, false if it already existed
    // (in which case the value is overwritten).
    bool insert(const K& key, const V& value) {
        if (loadFactor() > kMaxLoad) rehash(buckets_.size() * 2);
        const std::size_t b = bucketOf(key);
        for (Node* n = buckets_[b]; n; n = n->next) {
            if (n->key == key) { n->value = value; return false; }
        }
        buckets_[b] = new Node{key, value, buckets_[b]};
        ++size_;
        return true;
    }

    [[nodiscard]] std::optional<V> find(const K& key) const {
        for (const Node* n = buckets_[bucketOf(key)]; n; n = n->next)
            if (n->key == key) return n->value;
        return std::nullopt;
    }

    [[nodiscard]] bool contains(const K& key) const {
        return find(key).has_value();
    }

    bool erase(const K& key) {
        const std::size_t b = bucketOf(key);
        Node* prev = nullptr;
        for (Node* n = buckets_[b]; n; prev = n, n = n->next) {
            if (n->key == key) {
                (prev ? prev->next : buckets_[b]) = n->next;
                delete n;
                --size_;
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t bucketCount() const noexcept { return buckets_.size(); }
    [[nodiscard]] std::size_t rehashCount() const noexcept { return rehashes_; }
    [[nodiscard]] double loadFactor() const noexcept {
        return static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

    // How many nodes sit in bucket b -- the number the bucket diagram needs.
    [[nodiscard]] std::size_t bucketSize(std::size_t b) const {
        std::size_t n = 0;
        for (const Node* p = buckets_[b]; p; p = p->next) ++n;
        return n;
    }
    [[nodiscard]] std::size_t bucketOf(const K& key) const {
        return Hash{}(key) % buckets_.size();
    }

    void rehash(std::size_t newCount) {
        if (newCount <= buckets_.size()) return;
        std::vector<Node*> old(newCount, nullptr);
        buckets_.swap(old);
        const std::size_t keep = size_;
        size_ = 0;
        for (auto* head : old) {
            while (head) {
                Node* next = head->next;
                const std::size_t b = bucketOf(head->key);
                head->next = buckets_[b];
                buckets_[b] = head;
                ++size_;
                head = next;
            }
        }
        size_ = keep;
        ++rehashes_;
    }

private:
    static constexpr double kMaxLoad = 0.75;
    std::vector<Node*> buckets_;
    std::size_t size_ = 0;
    std::size_t rehashes_ = 0;
};

} // namespace pinjamlab::structures

#endif // PINJAMLAB_STRUCTURES_HASH_MAP_HPP
