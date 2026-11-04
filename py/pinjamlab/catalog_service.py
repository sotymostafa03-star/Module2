"""CatalogService: the read side of PinjamLab, over the same fixed CSV the
C++ tree loads. Sharing the input file is what makes the cross-language
comparison a measurement rather than an anecdote.
"""
from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

from .structures.repository import Repository


@dataclass(frozen=True)
class Item:
    tag: str
    model: str
    category: str
    daily_fine: int
    condition: str


class CatalogService:
    def __init__(self) -> None:
        self._repo: Repository[Item, str] = Repository(lambda i: i.tag)
        self._rejected = 0

    def load(self, path: str | Path) -> int:
        """Returns the number of rows accepted. Duplicate tags are rejected,
        not overwritten: one asset tag identifies one asset."""
        accepted = 0
        self._rejected = 0
        with open(path, newline="", encoding="utf-8") as fh:
            for row in csv.DictReader(fh):
                item = Item(
                    tag=row["tag"],
                    model=row["model"],
                    category=row["category"],
                    daily_fine=int(row["daily_fine"]),
                    condition=row["condition"],
                )
                if self._repo.add(item):
                    accepted += 1
                else:
                    self._rejected += 1
        return accepted

    def search(self, query: str) -> list[Item]:
        """Case-insensitive substring match over tag and model.

        O(n) by design: a substring search is not a key lookup, and no hash
        index can answer one. The Lab 2 defect is a CALLER that runs this
        inside a loop, not this function.
        """
        q = query.lower()
        return [i for i in self._repo.all()
                if q in i.tag.lower() or q in i.model.lower()]

    def by_category(self, category: str) -> list[Item]:
        return [i for i in self._repo.all() if i.category == category]

    def contains(self, tag: str) -> bool:
        return self._repo.contains(tag)

    def by_tag(self, tag: str) -> Optional[Item]:
        return self._repo.find_by_id(tag)

    @property
    def rejected(self) -> int:
        return self._rejected

    def __len__(self) -> int:
        return len(self._repo)
