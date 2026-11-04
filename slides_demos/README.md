# `slides_demos/` — the verified transcripts behind the Module 2 decks

Every `.cr-expected` block in `slides/Module2_Week*_slides.html` is the output
of one of these programs, produced by an actual run.
`MASTER_Presentation_Plan.md` §1: *if a number on a slide cannot be found in the
module or its `code/`, the slide is wrong, not the module.*

| Program | Deck · slide | What it shows |
|---|---|---|
| `cpp/m2c1_iterinvalid.cpp` | W4 · 9 | a `std::vector` reallocating, with the real before/after buffer addresses |
| `m2d1_mutate.py` | W4 · 10 | `dict` raising `RuntimeError`; `list` silently skipping half its elements |
| `cpp/m2c2_templateerror.cpp` | W5 · 5, 6 | **does not compile, on purpose** — the real 23-line `g++` template diagnostic for a key type with no `std::hash` |
| `cpp/m2c3_linkedlist.cpp` | W5 · 9 | the hero list, with node accounting: `alive == 0` at scope exit, and a deep copy proved by mutating one list only |
| `cpp/m2c4_buckets.cpp` | W5 · 11 | Pitfall C4 measured — same load factor, same correctness, 8× the comparisons |
| `cpp/m2c5_templatefixed.cpp` | W5 · 7 | the fix: supplying `std::hash<AssetTag>`, compiled and run |
| `m2d2_ducktyping.py` | W5 · 4 (live) | `Stack[int]` accepting a `str` with no error, failing one line later |
| `m2d3_listcost.py` | W5 · 12 | Lab 2.2's finding: the hand-built list is 8.8× slower than built-in `list` |
| `m2d4_accidental_on2.py` | W6 · 4 | the accidental O(n²): a correct O(n) query called once per category, measured in milliseconds against the same query served from one grouping pass |
| `m2d5_on2_counted.py` | W6 · 4 (live) | the same defect **counted** rather than timed — 4× rows ⇒ exactly 16× work |
| `cpp/m2c6_crossover.cpp` | W6 · 10 | Lab 3.4's harness — **and the anomaly that kept its number off the slide** |

The Week 4 benchmark slides (6, 7, 8) and the Week 6 chart slides (8, 9) quote
`../sample-data/bench-results.csv` directly rather than a transcript here —
it is produced by `./run_bench.sh` and committed as evidence.

## `m2d4_accidental_on2.py` — read this before quoting its numbers

It runs the same loop twice, at n = 500 and n = 2000, and prints both timings
plus how far each grew. **The timings move between runs**: two runs on the
authoring machine gave `scan x15.7 / index x4.1` and `scan x15.1 / index x2.0`,
because the indexed side measures about 11 ms against a microsecond-resolution
clock. What reproduces is the shape — **a 4× rise in n multiplies the scan's
time by roughly 16 and the indexed version's by roughly 4.** The committed
`transcripts/m2d4_accidental_on2.txt` is one real run, and the deck says so.

**The deck does not run this program live, and an earlier draft's attempt to is
the reason `m2d5_on2_counted.py` exists.** That draft put a *timed* block on
slide 4 whose `.cr-expected` quoted this file's numbers (`scan_all 2768.1 ms`)
while the block itself ran at n = 200 and printed `0.3 ms` — a claim the slide
contradicted the moment anyone pressed Run. A live timing can never match a
fixed expected transcript: it moves with the machine, the browser and the load.

So slide 4's live block counts **row touches** instead
(`m2d5_on2_counted.py`). $n \times 40 \times n$ is the same integer on every
laptop in the room, so the block's output and the slide's claim cannot drift
apart, and quadrupling n gives *exactly* 16× — a cleaner demonstration of the
shape than a timing anyway. This file stays as the wall-clock evidence that the
same defect costs real milliseconds, and the slide cites it as such.

## Reproducing

```bash
cd code/Module2
mkdir -p bin
g++ -std=c++20 -Wall -Wextra slides_demos/cpp/m2c1_iterinvalid.cpp -o bin/m2c1
./bin/m2c1 > slides_demos/transcripts/m2c1_iterinvalid.txt 2>&1

PYTHONPATH=py python slides_demos/m2d1_mutate.py \
  > slides_demos/transcripts/m2d1_mutate.txt 2>&1

python slides_demos/m2d4_accidental_on2.py \
  > slides_demos/transcripts/m2d4_accidental_on2.txt 2>&1
python slides_demos/m2d5_on2_counted.py \
  > slides_demos/transcripts/m2d5_on2_counted.txt 2>&1

# Week 5. The first of these is SUPPOSED to fail -- the error is the artifact.
g++ -std=c++20 -Icpp/include slides_demos/cpp/m2c2_templateerror.cpp -o /dev/null \
  > slides_demos/transcripts/m2c2_templateerror.txt 2>&1
for d in m2c3_linkedlist m2c4_buckets m2c5_templatefixed; do
  g++ -std=c++20 -Wall -Wextra -Icpp/include "slides_demos/cpp/$d.cpp" -o "bin/$d"
  "./bin/$d" > "slides_demos/transcripts/$d.txt" 2>&1
done
python slides_demos/m2d2_ducktyping.py > slides_demos/transcripts/m2d2_ducktyping.txt 2>&1
PYTHONPATH=py python slides_demos/m2d3_listcost.py \
  > slides_demos/transcripts/m2d3_listcost.txt 2>&1
```

`m2d2_ducktyping.py`, `m2d5_on2_counted.py` and `m2d4_accidental_on2.py` need no
`PYTHONPATH`: they are deliberately self-contained, because the first two are
blocks a student edits live on a slide. `m2d3_listcost.py` does need it — it
imports the committed `py/pinjamlab/structures/linked_list.py` on purpose, since
the number it reports is *about* that class.

## Which transcripts are NOT reproducible byte-for-byte

`m2c1_iterinvalid.txt` prints heap addresses (`buffer=000002017a602510`).
Address-space layout randomisation gives different values on every run, and the
byte offset the buffer moves by depends on `sizeof(std::string)` for your
standard library. **What must reproduce is the shape:** the buffer address is
unchanged when `size` reaches `capacity`, and different on the push after that.
The slide says so, so nobody in the room thinks the hex digits are the lesson.

`m2d4_accidental_on2.txt` and `m2d3_listcost.txt` print **timings**, so their
digits move too. For `m2d3` what must reproduce is the direction and the rough
size: the hand-built `LinkedList` loses to the built-in `list` by roughly an
order of magnitude (8.8× on the authoring machine), because one is a C array of
pointers and the other is a chain of Python objects reached by interpreted
attribute lookups.

`m2c6_crossover.txt` is **not a measurement at all — it is an investigation
log.** Its harness produced a `std::unordered_map::find` cost that rose with n
below n = 20 and then dropped discontinuously at n = 24, reproducibly and even
with a 20× larger batch. No crossover number from it appears on any slide;
Week 6 slide 10 cites only the bound the committed `bench-results.csv` supports
(the hash map already wins ~21× at n = 100, so the crossover is below 100) and
shows the anomaly itself as the lesson. If you later explain or fix it, that is
a genuine contribution — update the log rather than deleting it.

Everything else is designed to be identical on re-run: no timings in the
transcripts, no RNG, no iteration order that Python or the standard library is
free to change. `m2c2_templateerror.txt` embeds the authoring machine's
libstdc++ paths, which will differ on yours; the three lines the slide teaches
from do not.

## Keep the slide and the script in step

A projected code pane fits **about a dozen lines**, fewer when the slide also
carries a callout box. `m2d1_mutate.py` was compacted specifically to fit — its
two `if` bodies sit on one line each for that reason and no other. If you
lengthen it, re-run the deck check before teaching from it.

`m2d4_accidental_on2.py` is the mirror case: its body is **longer** than a
projected pane, on purpose. The slide shows the same loop shape at n = 200 as a
live, editable block; the file is the artifact that produced the numbers.

## Verified

- **Environment:** Windows 11 · g++ (GCC) 15.2.0 · Python 3.12.9 / 3.12.13
- **Date:** 2026-09-15
- **Result:** every program runs clean and `-Wall -Wextra` produces no warnings,
  with two deliberate exceptions:
  - `cpp/m2c2_templateerror.cpp` **does not compile, by design** — its 23-line
    diagnostic is the artifact Week 5 slides 5 and 6 teach from.
  - `cpp/m2c6_crossover.cpp` compiles and runs clean but its *output* is not
    trustworthy at small n (see above); it is committed as an investigation
    log, not as a measurement.
- `m2d4_accidental_on2.py` exits 0 and reports `same? yes` at both sizes, i.e.
  the scan and the indexed version return identical results.
- Both decks were re-checked headless (Playwright, Chromium) after these
  changes: **0 console errors, 0 overflowing slides, 0 clipped code panes** in
  both languages, and each deck's live Python block was executed under the
  vendored Pyodide and matched its `.cr-expected` byte for byte.
