"""Generic Repository[T, ID], dict-backed.

The C++ twin is a template resolved at compile time; this one is duck-typed
and resolved at call time. Both keep the same promise — `find_by_id` must not
scan — and only one of them tells you at build time when you break it.
"""
from __future__ import annotations

from typing import Callable, Generic, Iterable, Optional, TypeVar

T = TypeVar("T")
ID = TypeVar("ID")


class Repository(Generic[T, ID]):
    def __init__(self, id_of: Callable[[T], ID]) -> None:
        self._id_of = id_of
        self._items: list[T] = []
        self._index: dict[ID, int] = {}

    def add(self, item: T) -> bool:
        """False if the ID is already present; the item is NOT added."""
        key = self._id_of(item)
        if key in self._index:
            return False
        self._items.append(item)
        self._index[key] = len(self._items) - 1
        return True

    def find_by_id(self, key: ID) -> Optional[T]:
        pos = self._index.get(key)
        return None if pos is None else self._items[pos]

    def contains(self, key: ID) -> bool:
        return key in self._index

    def all(self) -> list[T]:
        return self._items

    def __len__(self) -> int:
        return len(self._items)

    def __iter__(self) -> Iterable[T]:
        return iter(self._items)
