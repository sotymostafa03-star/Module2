"""Singly linked list, ported from the C++ version.

The port exists to be *measured against* `list`, not to replace it. Python
already has a better sequence than this for almost every purpose; writing it
is how the pointer chasing becomes visible, and how the cost of one
allocation per node becomes a number rather than an opinion.
"""
from __future__ import annotations

from typing import Generic, Iterable, Iterator, Optional, TypeVar

T = TypeVar("T")


class Node(Generic[T]):
    __slots__ = ("value", "next")

    def __init__(self, value: T, nxt: Optional["Node[T]"] = None) -> None:
        self.value = value
        self.next = nxt


class LinkedList(Generic[T]):
    """No Rule of Five here: Python has no destructor to get wrong, and the
    reference count frees a dropped chain for you. That asymmetry with the C++
    header is the comparison Lab 3 asks you to write up."""

    def __init__(self, items: Iterable[T] | None = None) -> None:
        self._head: Optional[Node[T]] = None
        self._tail: Optional[Node[T]] = None
        self._size = 0
        for v in items or ():
            self.push_back(v)

    def push_front(self, value: T) -> None:
        self._head = Node(value, self._head)
        if self._tail is None:
            self._tail = self._head
        self._size += 1

    def push_back(self, value: T) -> None:
        node = Node(value)
        if self._tail is None:
            self._head = self._tail = node
        else:
            self._tail.next = node
            self._tail = node
        self._size += 1

    def insert_after(self, pos: int, value: T) -> bool:
        node = self._node_at(pos)
        if node is None:
            return False
        node.next = Node(value, node.next)
        if node is self._tail:
            self._tail = node.next
        self._size += 1
        return True

    def erase(self, pos: int) -> bool:
        if self._head is None or pos >= self._size:
            return False
        if pos == 0:
            self._head = self._head.next
            if self._head is None:
                self._tail = None
        else:
            prev = self._node_at(pos - 1)
            assert prev is not None and prev.next is not None
            doomed = prev.next
            prev.next = doomed.next
            if doomed is self._tail:
                self._tail = prev
        self._size -= 1
        return True

    def at(self, pos: int) -> Optional[T]:
        node = self._node_at(pos)
        return node.value if node else None

    def _node_at(self, pos: int) -> Optional[Node[T]]:
        node = self._head
        for _ in range(pos):
            if node is None:
                return None
            node = node.next
        return node

    def __iter__(self) -> Iterator[T]:
        node = self._head
        while node is not None:
            yield node.value
            node = node.next

    def __len__(self) -> int:
        return self._size

    def __contains__(self, value: object) -> bool:
        return any(v == value for v in self)

    def to_list(self) -> list[T]:
        return list(self)

    def __repr__(self) -> str:
        return f"LinkedList({self.to_list()!r})"
