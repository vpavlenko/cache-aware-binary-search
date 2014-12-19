This is sample code that shows how cache-oblivious binary search can be faster than a dumb one.

Sample output:
```
sorted_array: [ 0 8 18 22 31 ... 549961981 549961989 549961999 549962008 549962016 ]
query: [ 48270 182605793 407355682 192302370 353718329 ... 427913208 391098269 175850392 140336725 273669867 ]
slow, 100000 iterations: 160697772 ns
slow: 1606.98 ns per iteration
slow_result: [ 8776 33202324 74069554 34966153 64317415 ... 77805980 71114183 31973755 25515947 49763316 ]
chunk_size: 1000
inner_array_size: 100000
top_level_array: [ 0 5440 10899 16387 21903 ... 549934581 549940160 549945768 549951168 549956620 ]
fast, 100000 iterations: 93285257 ns
fast: 932.853 ns per iteration
fast_result: [ 8776 33202324 74069554 34966153 64317415 ... 77805980 71114183 31973755 25515947 49763316 ]
```

Issues
---

1. Show compiler version, compiler flags, operating system, CPU type and parameters.
2. Put model calculations that explain these numbers.
3. Attach disassembler output.
