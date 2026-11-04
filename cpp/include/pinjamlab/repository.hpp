#ifndef PINJAMLAB_REPOSITORY_HPP
#define PINJAMLAB_REPOSITORY_HPP

// Generic Repository<T, ID>: a vector of items plus a hand-built index from
// ID to position. The index is the whole point -- findById() must not scan.
//
// Templated, so the compiler resolves T and ID at COMPILE time: one class
// definition, one machine-code copy per instantiation, and a type error if
// the ID extractor does not match. Python's duck-typed twin in
// py/pinjamlab/structures/repository.py makes the same guarantee at run time,
// or not at all.

#include "pinjamlab/structures/hash_map.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

namespace pinjamlab {

template <typename T, typename ID, typename Hash = std::hash<ID>>
class Repository {
public:
    // The extractor is how a generic container stays ignorant of T's shape.
    using IdOf = std::function<ID(const T&)>;

    explicit Repository(IdOf idOf) : idOf_(std::move(idOf)) {}

    // Returns false if the ID is already present; the item is NOT added.
    // A repository that silently keeps two rows with one ID is the duplicate
    // bug Lab 2 asks you to find.
    bool add(const T& item) {
        const ID id = idOf_(item);
        if (index_.contains(id)) return false;
        items_.push_back(item);
        index_.insert(id, items_.size() - 1);
        return true;
    }

    [[nodiscard]] std::optional<T> findById(const ID& id) const {
        const auto pos = index_.find(id);
        if (!pos) return std::nullopt;
        return items_[*pos];
    }

    [[nodiscard]] bool contains(const ID& id) const { return index_.contains(id); }
    [[nodiscard]] const std::vector<T>& all() const noexcept { return items_; }
    [[nodiscard]] std::size_t size() const noexcept { return items_.size(); }

    // Exposed so the benchmark and the bucket diagram can read real numbers
    // out of the index rather than assert them.
    [[nodiscard]] const structures::HashMap<ID, std::size_t, Hash>& index() const noexcept {
        return index_;
    }

private:
    IdOf idOf_;
    std::vector<T> items_;
    structures::HashMap<ID, std::size_t, Hash> index_;
};

} // namespace pinjamlab

#endif // PINJAMLAB_REPOSITORY_HPP
