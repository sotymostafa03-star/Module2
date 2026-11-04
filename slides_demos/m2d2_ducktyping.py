"""Week 5 slide demo -- the same mistake as the C++ slide, in Python.

C++ refused to BUILD the HashMap whose key type lacked std::hash. Python
builds it, runs it, prints three lines of real output, and only then fails --
after the side effects have already happened. That is the whole trade.

Self-contained (no pinjamlab import) so the slide can run it live in Pyodide.
Compacted to fit a projected code pane -- about a dozen lines.

Run:  python slides_demos/m2d2_ducktyping.py
"""
class Stack[T]:                       # PEP 695 syntax, Python 3.12
    def __init__(self) -> None: self._items: list[T] = []
    def push(self, item: T) -> None: self._items.append(item)
    def total(self) -> T: return sum(self._items)

s: Stack[int] = Stack()
s.push(3); s.push(4)
print("pushed two ints, total =", s.total())
s.push("LP-001")                      # Stack[int].push(str) -- NOT an error
print("pushed a str into Stack[int]: no error, no warning, nothing")
try: print(s.total())
except TypeError as e: print("failure arrives HERE, not at the push:", e)
