"""The Python structures must agree with the built-ins they model, and the
CatalogService must agree with its C++ twin on the same committed CSV."""
from pathlib import Path

import pytest

from pinjamlab.catalog_service import CatalogService
from pinjamlab.structures.linked_list import LinkedList
from pinjamlab.structures.repository import Repository

CATALOG = Path(__file__).resolve().parents[2] / "sample-data" / "sample-catalog.csv"


def test_linked_list_matches_list_on_the_same_operations():
    ll = LinkedList(["a", "c"])
    ref = ["a", "c"]

    ll.insert_after(0, "b")
    ref.insert(1, "b")
    assert ll.to_list() == ref

    ll.push_front("start")
    ref.insert(0, "start")
    ll.push_back("end")
    ref.append("end")
    assert ll.to_list() == ref
    assert len(ll) == len(ref)


def test_linked_list_erase_keeps_head_tail_and_size_correct():
    ll = LinkedList([1, 2, 3, 4])
    assert ll.erase(0) and ll.to_list() == [2, 3, 4]
    assert ll.erase(len(ll) - 1) and ll.to_list() == [2, 3]
    ll.push_back(99)                       # proves the tail was fixed up
    assert ll.to_list() == [2, 3, 99]
    assert ll.erase(99) is False


def test_linked_list_membership_and_iteration():
    ll = LinkedList(range(5))
    assert 3 in ll and 99 not in ll
    assert [v * 2 for v in ll] == [0, 2, 4, 6, 8]
    assert ll.at(2) == 2 and ll.at(99) is None


def test_repository_rejects_a_duplicate_id():
    repo: Repository[str, str] = Repository(lambda s: s.split(":")[0])
    assert repo.add("LP-001:first") is True
    assert repo.add("LP-001:second") is False
    assert len(repo) == 1
    assert repo.find_by_id("LP-001") == "LP-001:first"
    assert repo.find_by_id("NOPE") is None


def test_catalogue_loads_all_twenty_committed_rows():
    svc = CatalogService()
    assert svc.load(CATALOG) == 20
    assert len(svc) == 20
    assert svc.rejected == 0


def test_duplicate_tags_are_rejected_not_overwritten():
    svc = CatalogService()
    svc.load(CATALOG)
    assert svc.by_tag("LP-001").model == "ThinkPad T14"
    assert svc.load(CATALOG) == 0           # every row collides
    assert svc.rejected == 20
    assert len(svc) == 20                   # still 20, not 40
    assert svc.by_tag("LP-001").model == "ThinkPad T14"   # the first row won


@pytest.mark.parametrize("query,expected", [
    ("thinkpad", 1), ("THINKPAD", 1), ("lp-", 5), ("zzz", 0),
])
def test_search_is_case_insensitive(query, expected):
    svc = CatalogService()
    svc.load(CATALOG)
    assert len(svc.search(query)) == expected


@pytest.mark.parametrize("category,expected", [
    ("Computing", 5), ("Optics", 5), ("Instrumentation", 5),
    ("Robotics", 3), ("Thermal", 2), ("Nonexistent", 0),
])
def test_by_category_partitions_the_catalogue(category, expected):
    svc = CatalogService()
    svc.load(CATALOG)
    assert len(svc.by_category(category)) == expected


def test_contains_agrees_with_by_tag():
    svc = CatalogService()
    svc.load(CATALOG)
    assert svc.contains("RA-003")
    assert svc.by_tag("RA-003").daily_fine == 120000
    assert not svc.contains("RA-999")
    assert svc.by_tag("RA-999") is None
