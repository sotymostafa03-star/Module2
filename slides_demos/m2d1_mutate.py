"""Week 4 slide demo — the same mistake in Python, and the two punishments.

C++ lets you walk off a reallocated buffer and says nothing. Python raises on
a dict and says nothing on a list. Neither language is "safe"; they just fail
in different places.

These are exactly the statements slide 10 runs — a projected code pane fits
about a dozen lines, so keep the two in step.

Run:  PYTHONPATH=py python slides_demos/m2d1_mutate.py
"""
shelf = {"LP-001": 1, "PR-014": 2, "LP-002": 3}
try:
    for tag in shelf:
        if tag.startswith("LP"): shelf[tag + "-COPY"] = 0
except RuntimeError as e:
    print("dict ->", type(e).__name__ + ":", e)

items = ["LP-001", "LP-002", "LP-003", "LP-004"]
for item in items:            # no copy: the index advances, the list shrinks
    if item.startswith("LP"): items.remove(item)
print("list -> no error, left with", items, "- expected [] if all were visited")
