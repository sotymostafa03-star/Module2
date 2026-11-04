from __future__ import annotations

import csv
from dataclasses import dataclass

from pinjamlab.structures.repository import Repository


@dataclass
class Item:
    tag: str
    model: str
    category: str
    daily_fine: int
    condition: str


class CatalogService:
    def __init__(self) -> None:
        self._repo = Repository(lambda item: item.tag)
        self._rejected = 0

    def load(self, path: str) -> int:
        accepted = 0
        self._rejected = 0

        with open(path, "r", encoding="utf-8", newline="") as file:
            reader = csv.DictReader(file)

            for row in reader:
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
        """Case-insensitive search with O(n) average-time deduplication."""
        q = query.lower()
        matches: list[Item] = []

        for item in self._repo.all():
            if q in item.tag.lower():
                matches.append(item)

        for item in self._repo.all():
            if q in item.model.lower():
                matches.append(item)

        seen: set[str] = set()
        unique: list[Item] = []

        for item in matches:
            if item.tag not in seen:
                seen.add(item.tag)
                unique.append(item)

        return unique

    def by_category(self, category: str) -> list[Item]:
        return [
            item
            for item in self._repo.all()
            if item.category == category
        ]

    def contains(self, tag: str) -> bool:
        return self._repo.contains(tag)

    def by_tag(self, tag: str) -> Item | None:
        return self._repo.find_by_id(tag)

    @property
    def rejected(self) -> int:
        return self._rejected

    def __len__(self) -> int:
        return len(self._repo)