# feather_libraries
[![Build Status](https://travis-ci.org/p-groarke/fea_libs.svg?branch=master)](https://travis-ci.org/p-groarke/fea_libs)
[![Build status](https://ci.appveyor.com/api/projects/status/olnrr5wjqpmgq2no/branch/master?svg=true)](https://ci.appveyor.com/project/p-groarke/fea-libs/branch/master)

A collection of useful libraries and utilities.


## Build
Install a recent CMake, Conan and compiler.

Tests are on by default, benchmarks are off by default.
Use `FEA_TESTS` and `FEA_BENCHMARKS` to enable/disable them.

CMake doesn't really detect your compiler's supported C++ standard well. Use `FEA_CPP14=On` to compile only the C++14 libraries.

Executables are output in `build/bin/`.

### Example
```
mkdir build && cd build
cmake .. -DFEA_TESTS=On -DFEA_BENCHMARKS=Off -DFEA_CPP14=Off && cmake --build .

// Optionally
cmake --build . --target install
```