## Overview
[![CMake on multiple platforms](https://github.com/mahdialmusaad/cutils/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/mahdialmusaad/cutils/actions/workflows/cmake-multi-platform.yml)

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

Version values for compilers are simplified into a major, minor and revision format.

### Functions
Functions are provided for tasks that simplify development or those that are OS/compiler specific:<br>

- String and path functions
- File management functions
- Cryptographic RNG
- Hardware and OS information functions
- Time-related functions
- Networking functions
- Threading and synchronization functions

An example of a chat server and client is provided in the following [file](examples/network.c).

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
Finally, there are some general macros for ease of development:

- Keywords (thread_local, restrict)
- Feature availability (keywords, compiler macros, builtins)
- Fixed-width integers (i32, u64, etc), with their respective formats and maximum values
- C version checking

The macros are also written in such a way to simplify checks:

```c
#if CU_OS_LINUX
/* Compiled on Linux */
#elif CU_COMPILER_ISVER(GCC, 8, 1)
/* Compiler is based on GCC 8.1 or higher */
#elif CU_ARCH_X86
/* Compiled for the x86 architecture */
```

## Support
This is mostly aimed towards being used for writing C code that can be compiled for Linux, MacOS and Windows as well as on major compilers without needing to write specific code for each - the source code is written in C89 and checks for feature support so any reasonable compiler should have no problem compiling it.

If you are attempting to build the library yourself but your system does not provide the required functionality, a warning will be emitted and the functions affected will not be available.

## Building
You can download the static library from the Releases section for each version. <br>
Alternatively, both [CMakeLists.txt](CMakeLists.txt) and a [Makefile](Makefile) are provided to build the library:

```bash
# Using CMake (omit '-DCU_BUILD_MISC=1' to only build the library):
$ cmake -B build -DCMAKE_BUILD_TYPE=Release -DCU_BUILD_MISC=1
$ cmake --build build --config Release

# Using make (omit 'all' to only build the library):
$ make all
```
Results can be found in the `build` directory. <br>
Additional documentation can be found in the aforementioned files.

## Sources
Most identification macros are from [predef](https://github.com/cpredef/predef) and [Boost](https://www.boost.org/doc/libs/latest/libs/predef/doc/index.html).<br>
Optimization macros and some attributes are from [Hedley](https://github.com/nemequ/hedley).<br>
Other macros are from own tests and may have incorrect detection.

## Changelog
### cu3
- Added thread pool/split, condition variables, directory listing and user/host name retrieval
- Various bug fixes relating to functions and macros
- Improved support for MacOS specific functionality
- Removed obsolete features
- Changed test and examples format

### cu2
- Networking functions added with chat server example
- custr_findnot, cu_res_osname and cu_thread_self added
- Reduced code size of string functions and fixed memory leaks for Unix functions
- Minor formatting changes to sources and debug allocation functions
- CMakeLists.txt added
- 'Releases' packaging changes

### cu1
- Initial release of cutils library
- Identification, debugging/optimization and QOL macros
- String/path, file, RNG, hardware, time, threading and allocation functions
