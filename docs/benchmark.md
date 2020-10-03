# fea_benchmark
Some benchmarks and benchmarking helper functions.

## Instructions
You can simply include bench_util.h in your program. It is a collection of functions inside `bench` namespace.

Alternatively, you can use cmake's `find_package`. In your `CMakeLists.txt` :
```
find_package(bench_util REQUIRED)
target_include_directories(${project_name} PRIVATE ${BENCH_UTIL_INCLUDE_DIRS})
```

### Cmake
You can build all examples and install the header in your user include path easily.
```
$ mkdir build && cd build && cmake .. && cmake --build . && cmake --build . --target install
```
This also installs the appropriate files so you can include the header lib using cmake `find_package`.

### Usage
```
#include <bench_util.h>

[...]

bench::title("Your title");
bench::start("Optional string");
// Do stuff
bench::clobber();
bench::stop("Optional but really recommended string");

bench::start();
// Do stuff
bench::clobber();
bench::stop("Optional but really recommended string");
```

outputs:

```
##########
Your title
##########


Optional string
---------------
Optional but really recommended string                                0.000000s
Optional but really recommended string                                0.000000s
```

### Disabling Output
A cmake option is available if you wish to disable console output completely.
```
-DBENCH_SHUTUP=On
```

You can turn off the benchmark output for a single file by defining `BENCH_SHUTUP 1` before you include the header.
```
#define BENCH_SHUTUP 1
#include <bench_util.h>
```

If you output information related to the benchmark, this output can be turned off along side the benchmarks. Use the `BENCH_PRINT` macro to print your messages. It is a wrapper on top of printf, and behaves the same way.
```
#define BENCH_SHUTUP 1
#include <bench_util.h>

/* Your code here ... */

BENCH_PRINT("Some int related to benchmarks %d\n", 42); // This wont output when you define BENCH_SHUTUP 1.

```

### Compiling
Of course, use your compiler optimizations when testing performance.

###### Windows
`cl /O2 main.cpp`

###### OS X
`clang++ -O3 -std=c++11 -stdlib=libc++ main.cpp`

### Useful commands
In most cases, you want to inspect various program attributes to understand the results. Here are a some useful commands.

###### Windows
- Generate assembly
  - /FA Assembly code; .asm
  - /FAc Machine and assembly code; .cod
  - /FAs Source and assembly code; .asm
  - /FAcs Machine, source, and assembly code; .cod
- Vtable layout
  - `cl /d1reportAllClassLayout main.cpp`
  - `cl /d1reportSingleClassLayoutYOURCLASS main.cpp`

###### OS X
- Generate assembly
  - `clang++ -S -std=c++11 -stdlib=libc++ -mllvm --x86-asm-syntax=intel main.cpp`
  - `clang++ -S -std=c++11 -stdlib=libc++ -masm=intel main.cpp` (Clang 3.5+)
- Vtable layout
  - `clang++ -cc1 -std=c++11 -stdlib=libc++ -fdump-record-layouts main.cpp`
- Memory layout (vtable + objects)
  - `clang++ -std=c++11 -stdlib=libc++ -Xclang -fdump-record-layouts -fsyntax-only main.cpp`
- Vtable with LLVM IR
  - `clang++ -std=c++11 -stdlib=libc++ -Xclang -fdump-record-layouts main.cpp`
- Dissasembly
  - `otool -tv a.out`

[More clang commands](http://clang.llvm.org/docs/CommandGuide/clang.html)

###### Linux
- Generate assembly
  - `g++ -S main.cpp`
  - `g++ -Wa,-adhln -g main.cpp` (Assembly + Source)
- Vtable layout
  - `g++ -fdump-class-hierarchy main.cpp`
- Dissasembly
  - `objdump -d a.out`

### Tools
In no specific order, various tools to profile results.

- [Zoom](http://www.rotateright.com) (Windows, Mac, Linux)
- [Valgrind Suite (Cachegrind, Memcheck)](http://valgrind.org/info/tools.html) (Mac, Linux)
- [Visual Studio Profiling Tools](https://msdn.microsoft.com/en-us/library/bb385770.aspx) (Windows)
- [Xcode Instruments](https://developer.apple.com/library/watchos/documentation/DeveloperTools/Conceptual/InstrumentsUserGuide/index.html) (Mac)
- [perf](https://perf.wiki.kernel.org/index.php/Main_Page) (Linux)
- [OProfile](http://oprofile.sourceforge.net) (Linux)
- [Intel Vtune](https://software.intel.com/en-us/intel-vtune-amplifier-xe) (Commercial - Windows, Linux)

[More tools](https://en.wikipedia.org/wiki/List_of_performance_analysis_tools)
