## Overview
A portable C utilities library.

This offers identification macros, wrappers for OS-specific functions and other QOL additions to your C project, simplifying writing applications by not having to worry about coding and testing multiple implementations for different systems.

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
Functions for common tasks that may have different implementations depending on the OS (or even compiler!). You can selectively disable certain function groups, or choose to only have the macros provided by the header.

The functions provided are as follows:

- String and path functions (find, replace and path traversal)
- File management functions
- Random number generation (pseudo and cryptographic)
- CPU and memory information retrieval
- Time and timing functions
- Threading and mutex functions
- Allocation debugging functions (detects buffer underruns, overruns and invalid calls)

Bit operation functions are also provided directly in the header. Compiler builtins are used if available, but a local implementation is used otherwise.

Warnings are given if you are compiling the library and a lack of support for specific functions are detected. Compilation can still continue, but the relevant functions will always fail, or may not be available.

### Optimization and Debugging
Macros are also available for simplifying optimizing and debugging your code:

- Assertion macros (compile and run-time)
- Function attributes (e.g. const, nothrow)
- Branch prediction and assumption macros
- Loop unrolling suggestions
- Compiler diagnostic macros
- Explicit breakpoints and traps

### QOL macros
Finally, there are some general macros for providing functionality which requires checking the compiler, headers and other things:

- Keywords (thread_local, restrict)
- Feature availability (keywords, compiler macros, builtins)
- Fixed-width integers (i32, u64, etc), with their respective printf formats and maximum values
- C version checking
- Common mathematical constants

The macros are also written in such a way to simplify checks (no need to write #ifdef everywhere), and helper macros are also provided to do so.

```c
/* Check if compiling on Linux, at least GCC 8.1, then the X86 architecture. */
#if CU_OS_LINUX
...
#elif CU_COMPILER_ISVER(GCC, 8, 1)
...
#elif CU_ARCH_X86
...
```

##### 

## Support
This is mostly aimed towards being used for creating applications that can be compiled for the Unix, MacOS and Windows operating systems as well as the major compilers without needing to write specific code for each, although most features are designed to be OS and compiler agnostic.

The CPU information (name, vendor, etc) only supports x86 CPUs as it requires support for the [cpuid](https://en.wikipedia.org/wiki/CPUID) instruction, but will compile for other architectures and will attempt to retrieve all other information possible without the instruction.

The source code is written in C89 so any reasonable compiler should have no problem compiling it. If you encounter any errors resulting from the code itself (incorrect results or compiler errors), it would be great if you open an issue or offer a solution.

## Building
A [Makefile](Makefile) is provided. Adjust the CFLAGS variable as you see fit and run `make` on the containing directory.
Alternatively, you can download the static library from the Releases section.

If you are compiling the library alongside your project, you only need to compile `cutils.c` and include the header file. However, you should set the `CU_SETTING_BUILDLIB` macro to 1 before including `cutils.h`.

Example:
```c
#define CU_SETTING_BUILDLIB 1
#include "cutils.h"
```
Or, more simply, you can set the macro directly in the compiler options:
```bash
$ gcc main.c cutils.c -DCU_SETTING_BUILD=1 ...
```

## Sources
Identification macros are from [predef](https://github.com/cpredef/predef) and [Boost](https://www.boost.org/doc/libs/latest/libs/predef/doc/index.html). <br>
Some attributes are sourced from [Hedley](https://github.com/nemequ/hedley).

Other macros are from own tests and may have incorrect detection(s). <br>
If you notice any instance of this, please create an issue.