# feather_libraries
[![Build Status]()
[![Build status]()

A collection of useful libraries and utilies.

## Build
Install a recent CMake, Conan and compiler.

Tests are on by default, benchmarks are off by default.
Use `FEA_TESTS` and `FEA_BENCHMARKS` to enable/disable them.

Executables are output in `build/bin/`.

### Example
```
mkdir build && cd build
cmake .. -DFEA_TESTS=Off -DFEA_BENCHMARKS=On && cmake --build .

// Optionally
cmake --build . --target install
```