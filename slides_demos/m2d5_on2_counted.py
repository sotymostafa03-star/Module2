"""Week 6 slide demo -- the accidental O(n^2) COUNTED, not timed.

Companion to m2d4_accidental_on2.py, which measures the same defect in
milliseconds. This one counts row-touches instead, for one reason: the week 6
slide runs it LIVE in front of the room, and a timing can never match a fixed
expected transcript -- it changes with the machine, the browser and the load.
A count cannot. n x 40 x n is the same integer everywhere, forever, so the
slide's claim and the slide's output can never drift apart.

Deterministic by construction: quadruple the rows and the scan does exactly
16x the work while the index does exactly 4x. That is the shape of O(n^2)
against O(n), with no clock involved.

Self-contained (no pinjamlab import) so Pyodide can run it from the slide.

Run:  python slides_demos/m2d5_on2_counted.py
"""
CATS = 40
def scan_report(units):        # the DEFECT: by_category() walks every row...
    touched = 0
    for _ in units:            # ...once per category, once per row
        for _c in range(CATS):
            touched += len(units)
    return touched

def indexed_report(units):     # group once, then one dict lookup per query
    groups = {}
    for t, c in units: groups.setdefault(c, []).append(t)
    return len(units) + len(units) * CATS

prev = None
for n in (500, 2000):
    u = [("T-%05d" % i, "CAT-%02d" % (i % CATS)) for i in range(n)]
    s, i = scan_report(u), indexed_report(u)
    print("n=%5d   scan touches %11d rows   indexed %7d" % (n, s, i))
    if prev: print("rows x%d  ->  scan x%d,  indexed x%d"
                   % (n // prev[0], s // prev[1], i // prev[2]))
    prev = (n, s, i)
