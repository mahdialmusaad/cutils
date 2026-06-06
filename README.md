## Overview
A portable C utilities library.

This offers identification macros, wrappers for OS-specific functions and other QOL additions for your C projects, simplifying writing applications by not having to worry about coding and testing multiple implementations for different systems.

## Features

### Identification macros
Macros are available for identifying the following:

- Compiler
- Operating system
- Architecture
- Platform
- Endianness and data model

Most version values for compilers are simplified into a major, minor and revision format.

### Functions
Functions are provided for tasks that simplify development or those that are OS/compiler specific. <br>
The functions provided are as follows:

- String and path functions (insert, replace, path traversal, etc)
- File management functions
- Random number generation (pseudo and cryptographic)
- Hardware information functions
- Time-related functions
- Networking functions (TCP, client and server)
- Threading and mutex functions
- Allocation debugging functions (detects buffer underruns, overruns and invalid calls)

Bit operation functions (e.g. popcount, count leading zeros) are also provided directly in the header.

An example of a chat server and client is provided in the following [file](net_test.c).

You can selectively disable certain function groups by defining their specific `CU_SETTING_*_FUNCS` macro to `0`:
```c
#define CU_SETTING_NETWORK_FUNCS 0
#include "cutils.h"
```

### Optimization and Debugging
Macros are also available for simplifying optimizing and debugging your code:

- Assertion macros (compile and run-time)
- Function attributes (e.g. const, nothrow)
- Branch prediction and assumption macros
- Compiler diagnostic macros
- Explicit breakpoints and traps

### QOL macros
Finally, there are some general macros for providing functionality which requires checking the compiler, headers and other things:

- Keywords (thread_local, restrict)
- Feature availability (keywords, compiler macros, builtins)
- Fixed-width integers (i32, u64, etc), with their respective printf formats and maximum values
- C version checking
- Common mathematical constants

The macros are also written in such a way to simplify checks (no need to check if defined), and helper macros are also provided to do so as well.

```c
#if CU_OS_LINUX
/* Compiled on Linux */
#elif CU_COMPILER_ISVER(GCC, 8, 1)
/* Compiler is based on GCC 8.1 or higher */
#elif CU_ARCH_X86
/* Compiled for the x86 architecture */
```

## Support
This is mostly aimed towards being used for creating applications that can be compiled for the Unix, MacOS and Windows operating systems as well as the major compilers without needing to write specific code for each, although most features are designed to be OS and compiler agnostic.

Warnings are given if you are compiling the library and a lack of support for specific functions are detected. Compilation can still continue, but the relevant functions will always fail (i.e. return 0), or may not be available.

The source code is written in C89 so any reasonable compiler should have no problem compiling it. If you encounter any errors resulting from the source code itself, it would be great if you open an issue or offer a solution.

## Building
You can download the static library from the Releases section for each version. <br>
Alternatively, both a [Makefile](Makefile) and [CMakeLists.txt](CMakeLists.txt) are provided to build the library:

```bash
# Using CMake (omit '-DCU_BUILD_MISC=1' to only build the library):
$ cmake -B build -DCMAKE_BUILD_TYPE=MinSizeRel -DCU_BUILD_MISC=1
$ cmake --build build --config MinSizeRel

# Using make (omit 'all' to only build the library):
$ make all
```
Results can be found in the `build` directory. <br>
Additional documentation can be found in the aforementioned files.

## Sources
Most identification macros are from [predef](https://github.com/cpredef/predef) and [Boost](https://www.boost.org/doc/libs/latest/libs/predef/doc/index.html). <br>
Optimization macros and some attributes are from [Hedley](https://github.com/nemequ/hedley). <br>
Other macros are from own tests and may have incorrect detection.

## Changelog
### cu2
- Networking functions added (cu_client_*, cu_server_* and cu_net_*) with chat server example
- custr_findnot, cu_res_osname and cu_thread_self added
- Reduced code size of string functions and fixed memory leaks for Unix functions
- Minor formatting changes to sources and debug allocation functions
- CMakeLists.txt added
- 'Releases' packaging changes

### cu1
- Initial release of cutils library
- Identification, debugging/optimization and QOL macros
- String/path, file, RNG, hardware, time, threading and allocation functions