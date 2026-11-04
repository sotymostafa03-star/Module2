// CatalogService behaviour over the fixed, committed sample-catalog.csv.
// Every number asserted here is a property of that file, so these tests are
// the same on every machine -- which is what makes "reproduce from clean"
// mean something.

#include "mini_catch.hpp"
#include "pinjamlab/catalog_service.hpp"

#include <string>

using namespace pinjamlab;

static CatalogService loaded() {
    CatalogService svc;
    svc.load("sample-data/sample-catalog.csv");
    return svc;
}

TEST_CASE("Catalogue loads all 20 committed rows") {
    CatalogService svc;
    const std::size_t n = svc.load("sample-data/sample-catalog.csv");
    REQUIRE(n == 20);
    REQUIRE(svc.size() == 20);
    REQUIRE(svc.rejected() == 0);
}

TEST_CASE("A duplicate tag is rejected, not overwritten") {
    CatalogService svc;
    svc.load("sample-data/sample-catalog.csv");
    const auto before = svc.byTag("LP-001");
    REQUIRE(before.has_value());
    REQUIRE(before->model == "ThinkPad T14");

    // Loading the same file again offers 20 rows that all collide.
    const std::size_t accepted = svc.load("sample-data/sample-catalog.csv");
    REQUIRE(accepted == 0);
    REQUIRE(svc.rejected() == 20);
    REQUIRE(svc.size() == 20);                    // still 20, not 40

    const auto after = svc.byTag("LP-001");
    REQUIRE(after.has_value());
    REQUIRE(after->model == "ThinkPad T14");      // the FIRST row won
}

TEST_CASE("search() is case-insensitive over tag and model") {
    const CatalogService svc = loaded();
    REQUIRE(svc.search("thinkpad").size() == 1);
    REQUIRE(svc.search("THINKPAD").size() == 1);
    REQUIRE(svc.search("lp-").size() == 5);       // LP-001..LP-005
    REQUIRE(svc.search("zzz").empty());
}

TEST_CASE("byCategory partitions the catalogue exactly") {
    const CatalogService svc = loaded();
    REQUIRE(svc.byCategory("Computing").size() == 5);
    REQUIRE(svc.byCategory("Optics").size() == 5);
    REQUIRE(svc.byCategory("Instrumentation").size() == 5);
    REQUIRE(svc.byCategory("Robotics").size() == 3);
    REQUIRE(svc.byCategory("Thermal").size() == 2);
    REQUIRE(svc.byCategory("Nonexistent").empty());
}

TEST_CASE("contains() goes through the index and agrees with byTag()") {
    const CatalogService svc = loaded();
    REQUIRE(svc.contains("RA-003"));
    REQUIRE(svc.byTag("RA-003").has_value());
    REQUIRE(svc.byTag("RA-003")->dailyFine == 120000);
    REQUIRE(!svc.contains("RA-999"));
    REQUIRE(!svc.byTag("RA-999").has_value());
}

int main() { return test_runner::runAll(); }
