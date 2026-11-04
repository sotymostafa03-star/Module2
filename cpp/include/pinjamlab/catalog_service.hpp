#ifndef PINJAMLAB_CATALOG_SERVICE_HPP
#define PINJAMLAB_CATALOG_SERVICE_HPP

// The read side of PinjamLab: load sample-catalog.csv once, then answer
// queries. search() is the diagnose-and-fix target in Lab 2 -- read its
// comment before you change it.

#include "pinjamlab/repository.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pinjamlab {

struct Item {
    std::string tag;
    std::string model;
    std::string category;
    int dailyFine = 0;
    std::string condition;
};

inline std::string toLower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

class CatalogService {
public:
    CatalogService() : repo_([](const Item& i) { return i.tag; }) {}

    // Returns the number of rows accepted. Rows with a duplicate tag are
    // REJECTED, not overwritten -- the catalogue is a set of assets, and one
    // asset tag identifies one asset.
    std::size_t load(const std::string& path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("cannot open catalogue: " + path);

        std::string line;
        if (!std::getline(in, line)) return 0;      // header
        std::size_t accepted = 0;
        rejected_ = 0;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::istringstream ls(line);
            Item it;
            std::string fine;
            std::getline(ls, it.tag, ',');
            std::getline(ls, it.model, ',');
            std::getline(ls, it.category, ',');
            std::getline(ls, fine, ',');
            std::getline(ls, it.condition, ',');
            it.dailyFine = fine.empty() ? 0 : std::stoi(fine);
            if (repo_.add(it)) ++accepted; else ++rejected_;
        }
        return accepted;
    }

    // Case-insensitive substring match over tag and model.
    //
    // This is O(n) by design: it is a SUBSTRING search, and no hash index can
    // answer one. Lab 2's diagnose-and-fix is NOT about making this O(1) --
    // it is about finding the caller that invokes it inside a loop.
    [[nodiscard]] std::vector<Item> search(const std::string& query) const {
        const std::string q = toLower(query);
        std::vector<Item> hits;
        for (const Item& it : repo_.all()) {
            if (toLower(it.tag).find(q) != std::string::npos ||
                toLower(it.model).find(q) != std::string::npos) {
                hits.push_back(it);
            }
        }
        return hits;
    }

    [[nodiscard]] std::vector<Item> byCategory(const std::string& category) const {
        std::vector<Item> hits;
        for (const Item& it : repo_.all())
            if (it.category == category) hits.push_back(it);
        return hits;
    }

    // O(1) average -- this one goes through the index, and that is the
    // difference the benchmark measures.
    [[nodiscard]] bool contains(const std::string& tag) const { return repo_.contains(tag); }
    [[nodiscard]] std::optional<Item> byTag(const std::string& tag) const {
        return repo_.findById(tag);
    }

    [[nodiscard]] std::size_t size() const noexcept { return repo_.size(); }
    [[nodiscard]] std::size_t rejected() const noexcept { return rejected_; }
    [[nodiscard]] const Repository<Item, std::string>& repo() const noexcept { return repo_; }

private:
    Repository<Item, std::string> repo_;
    std::size_t rejected_ = 0;
};

} // namespace pinjamlab

#endif // PINJAMLAB_CATALOG_SERVICE_HPP
