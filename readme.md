# feather_libraries
[![CI](https://github.com/p-groarke/fea_libs/actions/workflows/ci.yml/badge.svg)](https://github.com/p-groarke/fea_libs/actions/workflows/ci.yml)

A collection of useful libraries and utilities.

## Terminology
#### mt
*multi-threaded* - A threaded function or a threaded class.

If only a few functions of a class require threading, those functions offer a *\_mt* variation. When most of a class's APIs would be affected by using multi-threading, an alias of the class itself is provided.

#### cl
*captureless* - Use raw function pointers instead of `std::function`.

Many containers in feather libraries are functional in nature (state machines, event systems, etc). When storing user callbacks, the containers will default to the most flexible solution and use `std::function`. This can be bad for performance and can be a source of bugs due to capture lifetime mismanagement. When using *\_cl* versions of a class, it will use raw function pointers instead, preventing the usage of lambda captures or functors.

#### compact
*compact* - Use less memory, but tradeoff speed.

_Reserved for future uses, not implemented yet._ Some containers like `fea::fsm`, `fea::event_stack` and `fea::event_system` can become very heavy. Compact versions of these use indirections to save memory, but incurr runtime costs for doing so.


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
