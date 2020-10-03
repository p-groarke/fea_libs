# fea_unsigned_map
`unordered_map` like containers, optimized for unsigned keys.

Internal container and allocator are *not* customizable at this time.

## unsigned_map
`unsigned_map` is a slot map that follows the `unordered_map` c++ standard apis as close as possible. Unlike most slot map implementations, you provide the unique key. This is useful when retro-fitting slot maps into legacy code. If key generations are desirable for your use-case, use a `slot_map` instead.

**Warning : Key container will grow as large as the biggest key!**

### When To Use
* Keys grow from 0 to N.
* Each key is unique, aka a perfect hash.
* Memory is not a concearn or you can guarantee linearity in the keys (aka memory consumption isn't wasted).

### Features
* Has better copy/clear performance since it doesn't need rehashing.
* Has better value iteration performance since it doesn't use buckets and values are tightly packed.
* Insert is darn fast.
* Optimized for speed, not memory usage.


## flat_unsigned_hashmap
The `flat_unsigned_hashmap` is a more traditional hashing map, that has similar constraints as a slot map. Here, the lookup container will **not** grow as big as the biggest key. It is still much more performant than `unordered_map`, all the while offering a `data()` api which returns a pointer to the underlying values (hence flat).

### When To Use
* Keys aren't guaranteed to be unique, though as with all hashmaps, performance will degrade if there are too many collisions.
* You cannot guarantee key linearity (ex. pointers).
* You need to iterate contained values often.
* You must cross an ABI boundary, and wish to return a pointer to the data (or a `span`).

### Features
* Has overall better performance than an equivalent `unordered_map`.
* Doesn't use as much memory as `unsigned_map`, though it uses more memory than `unordered_map`.
* Data is stored contiguously.
* Access to underlying value buffer.

## Benchmarks
Benchmarks are available [here](unsigned_map_benchmarks.md)
