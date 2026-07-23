/*
   cutils - A portable C utilities library.
   Copyright (C) 2026 Mahdi Almusaad

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef CUTILS_HDR
#define CUTILS_HDR

#define CUTILS_VERSION 3

#ifdef __cplusplus
extern "C" {
#endif

/* ========================= User settings ======================== */

/* Whether to enable functions at all. */
#ifndef CU_SETTING_FUNCS
#  define CU_SETTING_FUNCS 1
#endif

/* Whether to enable certain function groups. */
#if CU_SETTING_FUNCS
#  ifndef CU_SETTING_STRING_FUNCS
#    define CU_SETTING_STRING_FUNCS 1
#  endif
#  ifndef CU_SETTING_CONTAINER_FUNCS
#    define CU_SETTING_CONTAINER_FUNCS 1
#  endif
#  ifndef CU_SETTING_FILE_FUNCS
#    define CU_SETTING_FILE_FUNCS 1
#  endif
#  ifndef CU_SETTING_RESOURCES_FUNCS
#    define CU_SETTING_RESOURCES_FUNCS 1
#  endif
#  ifndef CU_SETTING_TIME_FUNCS
#    define CU_SETTING_TIME_FUNCS 1
#  endif
#  ifndef CU_SETTING_NETWORK_FUNCS
#    define CU_SETTING_NETWORK_FUNCS 1
#  endif
#  ifndef CU_SETTING_THREAD_FUNCS
#    define CU_SETTING_THREAD_FUNCS 1
#  endif
#endif

/* Whether to typedef the numeric types (i32, u64, real32, etc).
   This needs the types to be defined elsewhere to compile. */
#ifndef CU_SETTING_TYPES
#  define CU_SETTING_TYPES 1
#endif

/* Whether to force CU_DEBUG, regardless of what mode the program was compiled with.
   This means assertions will not be disabled (CU_ASSERT). */
#ifndef CU_SETTING_FORCE_DEBUG
#  define CU_SETTING_FORCE_DEBUG 0
#endif

/* Whether to disable warning messages about unsupported functions. */
#ifndef CU_SETTING_QUIET
#  define CU_SETTING_QUIET 0
#endif

/* ========================= Compiler identification ======================== */

#define CU_COMPVER_MAKE(major, minor, patch) (((major) * 1000000) + ((minor) * 1000) + (patch))
#define CU_COMPVERP(compiler, major, minor, patch) (CU_COMP_ ## compiler >= CU_COMPVER_MAKE(major, minor, patch))
#define CU_COMPVER(compiler, major, minor) (CU_COMP_ ## compiler >= CU_COMPVER_MAKE(major, minor, 0))

#define CU_COMPILER_ISVER(compiler, major, minor) CU_COMPVER(compiler, major, minor)

#define CU_COMPILER_MAJOR(compiler) ((CU_COMP_ ## compiler) / 1000000)
#define CU_COMPILER_MINOR(compiler) (((CU_COMP_ ## compiler) / 1000) % 1000)
#define CU_COMPILER_PATCH(compiler) ((CU_COMP_ ## compiler) % 1000)

#ifdef __CC_ARM
#  define CU_COMP_ARM CU_COMPVER_MAKE(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
#  define CU_COMP_ARM CU_COMPVER_MAKE(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#else
#  define CU_COMP_ARM 0
#endif

#if defined(__BORLANDC__) || defined(__CODEGEARC__)
#  define CU_COMP_BORLAND CU_COMPVER_MAKE(__BORLANDC__ / 0x100, __BORLANDC__ % 0x100, 0)
#else
#  define CU_COMP_BORLAND 0
#endif

#ifdef __clang__
#  define CU_COMP_CLANG CU_COMPVER_MAKE(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#  define CU_COMP_CLANG 0
#endif

#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
#  define CU_COMP_GNU CU_COMPVER_MAKE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
#  define CU_COMP_GNU CU_COMPVER_MAKE(__GNUC__, __GNUC_MINOR__, 0)
#else
#  define CU_COMP_GNU 0
#endif

#ifdef __ibmxl__
#  define CU_COMP_IBM CU_COMPVER_MAKE(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined(__xlC__)
#  ifndef __xlC_ver__
#    define __xlC_ver__ 0
#  endif
#  define CU_COMP_IBM CU_COMPVER_MAKE(__xlC__ >> 8, __xlC__ & 0xFF, (__xlC_ver__ >> 8) & 0xFF)
#else
#  define CU_COMP_IBM 0
#endif

#if defined(__INTEL_COMPILER) && !defined(__ICL)
#  ifndef __INTEL_COMPILER_UPDATE
#    define __INTEL_COMPILER_UPDATE 0
#  endif
#  define CU_COMP_INTEL CU_COMPVER_MAKE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#else
#  define CU_COMP_INTEL 0
#endif

#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 1000)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 10)
#elif defined(_MSC_VER) && !defined(__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_VER / 100, _MSC_VER % 100, 0)
#else
#  define CU_COMP_MSVC 0
#endif

#ifdef __NVCC__
#  define CU_COMP_NVCC CU_COMPVER_MAKE(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
#else
#  define CU_COMP_NVCC 0
#endif

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#  if defined(__SUNPRO_C)
#    define __SUNVER __SUNPRO_C
#  else
#    define __SUNVER __SUNPRO_CC
#  endif
#  if __SUNVER > 0x1000
#    define CU_COMP_SUNPRO CU_COMPVER_MAKE((((__SUNVER >> 16) & 0xf) * 10) + ((__SUNVER >> 12) & 0xf), (((__SUNVER >> 8) & 0xf) * 10) + ((__SUNVER >> 4) & 0xf), (__SUNVER & 0xf) * 10)
#  else
#    define CU_COMP_SUNPRO CU_COMPVER_MAKE((__SUNVER >> 8) & 0xf, (__SUNVER >> 4) & 0xf, (__SUNVER) & 0xf)
#  endif
#else
#  define CU_COMP_SUNPRO 0
#endif

#define CU_COMP_GCC CU_COMP_GNU

/* ========================= OS identification ======================== */

#ifdef _AIX
#  ifdef __AIX43
#    define CU_OS_AIX 43
#  elif defined(__AIX41)
#    define CU_OS_AIX 41
#  else
#    define CU_OS_AIX 32
#  endif
#else
#  define CU_OS_AIX 0
#endif

#if defined(AMIGA) || defined(__amigaos__)
#  define CU_OS_AMIGA 1
#else
#  define CU_OS_AMIGA 0
#endif

#if defined(BSD) || defined(_SYSTYPE_BSD)
#  ifdef BSD4_4
#    define CU_OS_BSD 44
#  elif defined(BSD4_3)
#    define CU_OS_BSD 43
#  elif defined(BSD4_2)
#    define CU_OS_BSD 42
#  else
#    define CU_OS_BSD BSD
#  endif
#else
#  define CU_OS_BSD 0
#endif

#ifdef __bsdi__
#  define CU_OS_BSDI 1
#else
#  define CU_OS_BSDI 0
#endif

#ifdef __DragonFly__
#  define CU_OS_BSD_DRAGONFLY 1
#else
#  define CU_OS_BSD_DRAGONFLY 0
#endif

#ifdef __FreeBSD__
#  define CU_OS_BSD_FREE __FreeBSD_version
#else
#  define CU_OS_BSD_FREE 0
#endif

#if defined(__NetBSD__) || defined(__NETBSD__)
#  ifdef NetBSD0_8
#    define CU_OS_BSD_NET 8
#  elif defined(NetBSD0_9)
#    define CU_OS_BSD_NET 9
#  elif defined(NetBSD1_0)
#    define CU_OS_BSD_NET 10
#  else
#    define CU_OS_BSD_NET __NetBSD_Version
#  endif
#else
#  define CU_OS_BSD_NET 0
#endif

#ifdef __OpenBSD__
#  ifdef OpenBSD5_0
#    define CU_OS_BSD_OPEN 50
#  elif defined(OpenBSD5_1)
#    define CU_OS_BSD_OPEN 51
#  elif defined(OpenBSD5_5)
#    define CU_OS_BSD_OPEN 55
#  elif defined(OpenBSD5_3)
#    define CU_OS_BSD_OPEN 53
#  elif defined(OpenBSD5_4)
#    define CU_OS_BSD_OPEN 54
#  elif defined(OpenBSD5_5)
#    define CU_OS_BSD_OPEN 55
#  elif defined(OpenBSD5_6)
#    define CU_OS_BSD_OPEN 56
#  elif defined(OpenBSD5_7)
#    define CU_OS_BSD_OPEN 57
#  elif defined(OpenBSD5_8)
#    define CU_OS_BSD_OPEN 58
#  elif defined(OpenBSD5_9)
#    define CU_OS_BSD_OPEN 59
#  elif defined(OpenBSD6_0)
#    define CU_OS_BSD_OPEN 60
#  elif defined(OpenBSD6_1)
#    define CU_OS_BSD_OPEN 61
#  elif defined(OpenBSD6_6)
#    define CU_OS_BSD_OPEN 66
#  elif defined(OpenBSD6_3)
#    define CU_OS_BSD_OPEN 63
#  elif defined(OpenBSD6_4)
#    define CU_OS_BSD_OPEN 64
#  elif defined(OpenBSD6_5)
#    define CU_OS_BSD_OPEN 65
#  elif defined(OpenBSD6_6)
#    define CU_OS_BSD_OPEN 66
#  elif defined(OpenBSD6_7)
#    define CU_OS_BSD_OPEN 67
#  elif defined(OpenBSD6_8)
#    define CU_OS_BSD_OPEN 68
#  elif defined(OpenBSD6_9)
#    define CU_OS_BSD_OPEN 69
#  elif defined(OpenBSD7_0)
#    define CU_OS_BSD_OPEN 70
#  elif defined(OpenBSD7_1)
#    define CU_OS_BSD_OPEN 71
#  elif defined(OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined(OpenBSD7_3)
#    define CU_OS_BSD_OPEN 73
#  elif defined(OpenBSD7_4)
#    define CU_OS_BSD_OPEN 74
#  elif defined(OpenBSD7_5)
#    define CU_OS_BSD_OPEN 75
#  elif defined(OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined(OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined(OpenBSD7_8)
#    define CU_OS_BSD_OPEN 78
#  elif defined(OpenBSD7_9)
#    define CU_OS_BSD_OPEN 79
#  else
#    define CU_OS_BSD_OPEN 1
#  endif
#else
#  define CU_OS_BSD_OPEN 0
#endif

#ifdef __CYGWIN__
#  define CU_OS_CYGWIN 1
#else
#  define CU_OS_CYGWIN 0
#endif

#ifdef __HAIKU__
#  define CU_OS_HAIKU 1
#else
#  define CU_OS_HAIKU 0
#endif

#if defined(_hpux) || defined(hpux) || defined(__hpux)
#  define CU_OS_HPUX 1
#else
#  define CU_OS_HPUX 0
#endif

#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) && (defined(__APPLE__) || defined(__MACH__))
#  define CU_OS_IOS __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ * 1000
#else
#  define CU_OS_IOS 0
#endif

#if defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
#  define CU_OS_LINUX 1
#else
#  define CU_OS_LINUX 0
#endif

#if defined(macintosh) || defined(Macintosh) || defined(__APPLE__) || defined(__MACH__)
#  if defined(__APPLE__) && defined(__MACH__)
#    define CU_OS_MAC 10
#  else
#    define CU_OS_MAC 9
#  endif
#else
#  define CU_OS_MAC 0
#endif

#if defined(__OS400__) || defined(__OS400_TGTVRM__)
#  define CU_OS_IBM400 1
#else
#  define CU_OS_IBM400 0
#endif

#if defined(__QNX__) || defined(__QNXNTO__) || defined(_NTO_VERSION)
#  include <sys/neutrino.h>
#  define CU_OS_QNX _NTO_VERSION
#else
#  define CU_OS_QNX 0
#endif

#if defined(sun) || defined(__sun)
#  if CU_OS_SVR4
#    define CU_OS_SOLARIS 1
#  else
#    define CU_OS_SUN 1
#  endif
#else
#  define CU_OS_SOLARIS 0
#  define CU_OS_SUN 1
#endif

#if defined(__unix__) || defined(__unix) || defined(unix) || \
    defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE) || CU_OS_MAC
#  define CU_OS_UNIX 1
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  ifndef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS 64
#  endif
#else
#  define CU_OS_UNIX 0
#endif

#if defined(VMS) || defined(__VMS) || defined(__VMS_VER)
#  define CU_OS_VMS __VMS_VER
#else
#  define CU_OS_VMS 0
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__) || defined(__MINGW32__) || defined(__MINGW64__)
#  define CU_OS_WINDOWS 1
#  define WIN32_LEAN_AND_MEAN
#  ifdef _WIN64
#    define CU_OS_WIN64 1
#    define CU_OS_WIN32 0
#    define CU_OS_64BIT 1
#  else
#    define CU_OS_WIN64 0
#    define CU_OS_WIN32 1
#    define CU_OS_32BIT 1
#  endif
#else
#  define CU_OS_WINDOWS 0
#  define CU_OS_WIN64 0
#  define CU_OS_WIN32 0
#endif

#ifndef CU_OS_64BIT
#  define CU_OS_64BIT 0
#endif
#ifndef CU_OS_32BIT
#  define CU_OS_32BIT 0
#endif

/* ========================= Architecture identification ======================== */

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#  ifdef __alpha_ev6__
#    define CU_ARCH_ALPHA 600
#  elif defined(__alpha_ev5__)
#    define CU_ARCH_ALPHA 500
#  elif defined(__alpha_ev4__)
#    define CU_ARCH_ALPHA 400
#  else
#    define CU_ARCH_ALPHA 1
#  endif
#else
#  define CU_ARCH_ALPHA 0
#endif

#if defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || \
    defined(_M_ARM) || defined(_M_ARM64) || defined(__ARM_ARCH) || \
    defined(__arm__) || defined(__arm64) || defined(__arm64__) || \
    defined(__thumb__) || defined(__aarch64__) || defined(__AARCH64EL__)
#  ifdef __ARM_ARCH_2__
#    define CU_ARCH_ARM 200
#  elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#    define CU_ARCH_ARM 300
#  elif defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#    define CU_ARCH_ARM 400
#  elif defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_5T__)
#    define CU_ARCH_ARM 500
#  elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || \
        defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) || \
	defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6Z__) || \
	defined(__ARM_ARCH_6K__)
#    define CU_ARCH_ARM 600
#  elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
        defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
	defined(__ARM_ARCH_7S__)
#    define CU_ARCH_ARM 700
#  elif defined(__AARCH64EL__) || defined(__aarch64__)
#    define CU_ARCH_ARM 800
#    define CU_ARCH_ARM_AARCH64 1
#    define CU_ARCH_64BIT 1
#  else
#    define CU_ARCH_ARM 1
#  endif
#  ifndef CU_ARCH_ARM_AARCH64
#    define CU_ARCH_ARM_AARCH64 0
#  endif
#  ifdef __thumb__
#    define CU_ARCH_ARM_THUMB 1
#  else
#    define CU_ARCH_ARM_THUMB 0
#  endif
#  if !defined(CU_ARCH_64BIT) && (defined(__arm64) || defined(__arm64__) || defined(_M_ARM64))
#    define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_ARM 0
#  define CU_ARCH_ARM_THUMB 0
#  define CU_ARCH_ARM_AARCH64 0
#endif

#if defined(__e2k__) || defined(__iset__)
#  ifdef __elbrus_16c__
#    define CU_ARCH_E2K 1600
#  elif defined(__elbrus_12c__)
#    define CU_ARCH_E2K 1200
#  elif defined(__elbrus_2c3__)
#    define CU_ARCH_E2K 230
#  elif defined(__elbrus_8c__)
#    define CU_ARCH_E2K 80
#  else
#    define CU_ARCH_E2K 1
#  endif
#else
#  define CU_ARCH_E2K 0
#endif

#if defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) || defined(__itanium__)
#  define CU_ARCH_IA64 1
#  ifndef CU_ARCH_64BIT
#    define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_IA64 0
#endif

#if defined(__loongarch__) || defined(__loongarch_arch) || defined(__loongarch_tune)
#  ifdef __loongarch_arch
#    define CU_ARCH_LOONGARCH __loongarch_arch
#  elif defined(__loongarch_tune)
#    define CU_ARCH_LOONGARCH __loongarch_tune
#  else
#    define CU_ARCH_LOONGARCH 1
#  endif
#else
#  define CU_ARCH_LOONGARCH 0
#endif

#if defined(__mips__) || defined(__mips) || defined(__MIPS__)
#  if defined(__MIPS_ISA4__) || defined(_MIPS_ISA_MIPS4)
#    define CU_ARCH_MIPS 400
#  elif defined(__MIPS_ISA3__) || defined(_MIPS_ISA_MIPS3)
#    define CU_ARCH_MIPS 300
#  elif defined(__MIPS_ISA2__) || defined(_MIPS_ISA_MIPS2) || defined(_R4000)
#    define CU_ARCH_MIPS 200
#  elif defined(__MIPS_ISA1__) || defined(_MIPS_ISA_MIPS1) || defined(_R3000)
#    define CU_ARCH_MIPS 100
#  else
#    define CU_ARCH_MIPS 1
#  endif
#else
#  define CU_ARCH_MIPS 0
#endif

#if defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || \
    defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(__PPCGECKO__) || \
    defined(__POWERPC__) || defined(__ppc__) || defined(__ppc64__) || \
    defined(__PPCBROADWAY__) || defined(_XENON) || defined(__ppc) || \
    defined(__PPC__) || defined(__PPC64__) || defined(_M_PPC)
#  if defined(__ppc604__) || defined(_ARCH_604)
#    define CU_ARCH_PPC 604
#  elif defined(__ppc603__) || defined(_ARCH_603)
#    define CU_ARCH_PPC 603
#  elif defined(__ppc601__) || defined(_ARCH_601)
#    define CU_ARCH_PPC 601
#  elif defined(_ARCH_450)
#    define CU_ARCH_PPC 450
#  elif defined(_ARCH_440)
#    define CU_ARCH_PPC 440
#  else
#    define CU_ARCH_PPC 620
#  endif
#  if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#      define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_PPC 0
#endif

#ifdef __CUDA_ARCH__
#  define CU_ARCH_PTX __CUDA_ARCH__
#else
#  define CU_ARCH_PTX 0
#endif

#if defined(__riscv) || defined(__riscv_32) || defined(__riscv_64)
#  if defined(__riscv_64) && !defined(CU_ARCH_64BIT)
#    define CU_ARCH_64BIT 1
#  elif defined(__riscv_32) && !defined(CU_ARCH_32BIT)
#    define CU_ARCH_32BIT 1
#  endif
#  define CU_ARCH_RISCV 1
#else
#  define CU_ARCH_RISCV 0
#endif

#if defined(__sparc__) || defined(__sparc)
#  if defined(__sparcv9) || defined(__sparc_v9__)
#    define CU_ARCH_SPARC 900
#  elif defined(__sparcv8) || defined(__sparc_v8__)
#    define CU_ARCH_SPARC 800
#  else
#    define CU_ARCH_SPARC 1
#  endif
#else
#  define CU_ARCH_SPARC 0
#endif

#ifdef __sh__
#  ifdef __SH5__
#    define CU_ARCH_SH 500
#  elif defined(__SH4__)
#    define CU_ARCH_SH 400
#  elif defined(__sh3__) || defined(__SH3__)
#    define CU_ARCH_SH 300
#  elif defined(__sh2__)
#    define CU_ARCH_SH 200
#  elif defined(__sh1__)
#    define CU_ARCH_SH 100
#  else
#    define CU_ARCH_SH 1
#  endif
#else
#  define CU_ARCH_SH 0
#endif

#if defined(__x86_64) || defined(__x86_64__) || defined(_M_X64)
#  define CU_ARCH_X86 700
#  ifndef CU_ARCH_64BIT
#    define CU_ARCH_64BIT 1
#  endif
#elif defined(i386) || defined(__i386__) || defined(__i386) || \
    defined(__i486__) || defined(__i586__) || defined(__i686__) || \
    defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL__) || \
    defined(__I86__) || defined(__INTEL__)
#  ifdef _M_IX86
#    define CU_ARCH_X86 _M_IX86
#  elif defined(__i686__) || defined(__I86__)
#    define CU_ARCH_X86 600
#  elif defined(__i586__)
#    define CU_ARCH_X86 500
#  elif defined(__i486__)
#    define CU_ARCH_X86 400
#  elif defined(__i386__)
#    define CU_ARCH_X86 300
#  else
#    define CU_ARCH_X86 1
#  endif
#  define CU_ARCH_32BIT 1
#else
#  define CU_ARCH_X86 0
#endif

#ifdef __SYSC_ZARCH__
#  define CU_ARCH_Z 1
#else
#  define CU_ARCH_Z 0
#endif

#ifndef CU_ARCH_64BIT
#  define CU_ARCH_64BIT 0
#endif
#ifndef CU_ARCH_32BIT
#  define CU_ARCH_32BIT 0
#endif

/* ========================= Platform identification ======================== */

#ifdef __ANDROID__
#  include <android/api-level.h>
#  define CU_PLAT_ANDROID __ANDROID_API__
#else
#  define CU_PLAT_ANDROID 0
#endif

#ifdef __CloudABI__
#  define CU_PLAT_CLOUDABI 1
#else
#  define CU_PLAT_CLOUDABI 0
#endif

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_SIMULATOR) || \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#  define CU_PLAT_IOS 1
#else
#  define CU_PLAT_IOS 0
#endif

#if defined (__MINGW32__) || defined (__MINGW64__)
#  define CU_PLAT_MINGW 1
#  if defined (__MINGW64__)
#    ifdef __MINGW64_VERSION_MAJOR
#      define CU_PLAT_MINGW64 CU_COMPVER_MAKE(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
#      define CU_PLAT_MINGW32 0
#    else
#      define CU_PLAT_MINGW64 1
#      define CU_PLAT_MINGW32 0
#    endif
#  else
#    ifdef __MINGW32_VERSION_MAJOR
#      define CU_PLAT_MINGW64 0
#      define CU_PLAT_MINGW32 CU_COMPVER_MAKE(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#    else
#      define CU_PLAT_MINGW64 0
#      define CU_PLAT_MINGW32 1
#    endif
#  endif
#else
#  define CU_PLAT_MINGW 0
#  define CU_PLAT_MINGW32 0
#  define CU_PLAT_MINGW64 0
#endif

#ifdef WINAPI_FAMILY
#  define CU_PLAT_WIN 1
#  if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#   define CU_PLAT_WINDESK 1
#   define CU_PLAT_WINPHONE 0
#   define CU_PLAT_WINSERVER 0
#   define CU_PLAT_WINSTORE 0
#   define CU_PLAT_WINSYS 0
#  elif WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#   define CU_PLAT_WINDESK 0
#   define CU_PLAT_WINPHONE 1
#   define CU_PLAT_WINSERVER 0
#   define CU_PLAT_WINSTORE 0
#   define CU_PLAT_WINSYS 0
#  elif WINAPI_FAMILY == WINAPI_FAMILY_SERVER
#   define CU_PLAT_WINDESK 0
#   define CU_PLAT_WINPHONE 0
#   define CU_PLAT_WINSERVER 1
#   define CU_PLAT_WINSTORE 0
#   define CU_PLAT_WINSYS 0
#  elif WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
#   define CU_PLAT_WINDESK 0
#   define CU_PLAT_WINPHONE 0
#   define CU_PLAT_WINSERVER 0
#   define CU_PLAT_WINSTORE 1
#   define CU_PLAT_WINSYS 0
#  elif WINAPI_FAMILY == WINAPI_FAMILY_SYSTEM
#   define CU_PLAT_WINDESK 0
#   define CU_PLAT_WINPHONE 0
#   define CU_PLAT_WINSERVER 0
#   define CU_PLAT_WINSTORE 0
#   define CU_PLAT_WINSYS 1
#  endif
#  include <winapifamily.h>
#  if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#    define CU_PLAT_UWP 1
#  else
#    define CU_PLAT_UWP 0
#  endif
#else
#  define CU_PLAT_WINDESK 0
#  define CU_PLAT_WINPHONE 0
#  define CU_PLAT_WINSERVER 0
#  define CU_PLAT_WINSTORE 0
#  define CU_PLAT_WINSYS 0
#  define CU_PLAT_UWP 0
#endif

/* ========================= Data model ======================== */

#if (defined(__fourbyteints__) && !(__fourbyteints__ - 0))
#  define CU_DM_LP32 1
#else
#  define CU_DM_LP32 0
#endif

#if (CU_COMP_MSVC && CU_OS_WIN32) || CU_PLAT_MINGW32 || defined(_ILP32) || defined(__ILP32__) || (defined(__fourbyteints__) && (__fourbyteints__ - 0))
#  define CU_DM_ILP32 1
#else
#  define CU_DM_ILP32 0
#endif

#if defined(_LP64) || defined(__LP64__)
#  define CU_DM_LP64 1
#else
#  define CU_DM_LP64 0
#endif

#if defined(__LLP64__) || (CU_COMP_MSVC && CU_OS_WIN64) || CU_PLAT_MINGW64 || (CU_COMP_CLANG && CU_OS_MAC)
#  define CU_DM_LLP64 1
#else
#  define CU_DM_LLP64 0
#endif

#if CU_DM_LLP64 || CU_DM_LP64 || CU_ARCH_64BIT || CU_OS_64BIT
#  define CU_DM_64BIT 1
#  define CU_DM_32BIT 0
#else
#  define CU_DM_64BIT 0
#  define CU_DM_32BIT 1
#endif

#if CU_DM_LLP64 || CU_DM_32BIT
#  define CU_DM_LONGSUF ll
#  define CU_DM_LONGSUF_CAP LL
#  define CU_DM_LL 1
#else
#  define CU_DM_LONGSUF l
#  define CU_DM_LONGSUF_CAP L
#  define CU_DM_LL 0
#endif

/* ========================= Language features ======================== */

#define CU_LANG_C23 202311L
#define CU_LANG_C17 201710L
#define CU_LANG_C11 201112L
#define CU_LANG_C99 199901L
#define CU_LANG_C95 199409L
#define CU_LANG_C89 198900L
#define CU_LANG_CKR 197200L

#ifdef __STDC__
#  ifdef __STDC_VERSION__
#    define CU_LANG_C __STDC_VERSION__
#  else
#    define CU_LANG_C CU_LANG_C89
#  endif
#else
#  define CU_LANG_C CU_LANG_CKR
#endif

#if CU_COMPVER(GNU, 2, 8) || CU_COMP_CLANG
#  define CU_GNU_EXT __extension__
#else
#  define CU_GNU_EXT
#endif

#if CU_LANG_C >= CU_LANG_C23
#  define CU_TYPEOF(x) typeof(x)
#  define CU_TYPEOF_AVAILABLE 1
#elif CU_COMP_GNU
#  define CU_TYPEOF(x) __typeof__(x)
#  define CU_TYPEOF_AVAILABLE 1
#else
#  define CU_TYPEOF(x)
#  define CU_TYPEOF_AVAILABLE 0
#endif

#if CU_COMP_MSVC
#  define CU_ASM __asm
#else
#  define CU_ASM __asm__
#endif

#ifdef __STDC_THREADS__
#  define CU_THREAD_C_AVAILABLE 1
#else
#  define CU_THREAD_C_AVAILABLE 0
#endif

#if CU_OS_WINDOWS
#  define CU_THREAD_WIN_AVAILABLE 1
#else
#  define CU_THREAD_WIN_AVAILABLE 0
#endif

#if CU_OS_UNIX || defined(_POSIX_THREADS) || defined(__EMSCRIPTEN_PTHREADS__)
#  define CU_THREAD_POSIX_AVAILABLE 1
#else
#  define CU_THREAD_POSIX_AVAILABLE 0
#endif

#define CU_CONCAT3_DETAIL(a,b,c) a ## b ## c
#define CU_CONCAT_DETAIL(a,b) a ## b
#define CU_STRINGIFY_DETAIL(a) #a

#define CU_CONCAT3(a,b,c) CU_CONCAT3_DETAIL(a,b,c)
#define CU_CONCAT(a,b) CU_CONCAT_DETAIL(a,b)
#define CU_STRINGIFY(a) CU_STRINGIFY_DETAIL(a)
#define CU_EVAL(a) a

#ifdef __has_include
#  define CU_HAS_INCLUDE(inc) __has_include(inc)
#  define CU_HAS_INCLUDE_AVAILABLE 1
#else
#  define CU_HAS_INCLUDE(inc) 0
#  define CU_HAS_INCLUDE_AVAILABLE 0
#endif

#ifdef __has_include_next
#  define CU_HAS_INCLUDE_NEXT(inc) __has_include_next(inc)
#  define CU_HAS_INCLUDE_NEXT_AVAILABLE 1
#else
#  define CU_HAS_INCLUDE_NEXT(inc) 0
#  define CU_HAS_INCLUDE_NEXT_AVAILABLE 0
#endif

#if defined(__has_attribute)
#  define CU_HAS_ATTRIBUTE(attrib) __has_attribute(attrib)
#  define CU_HAS_ATTRIBUTE_AVAILABLE 1
#else
#  define CU_HAS_ATTRIBUTE(attrib) 0
#  define CU_HAS_ATTRIBUTE_AVAILABLE 0
#endif

#ifdef __has_builtin
#  define CU_HAS_BUILTIN(builtin) __has_builtin(builtin)
#  define CU_HAS_BUILTIN_AVAILABLE 1
#else
#  define CU_HAS_BUILTIN(builtin) 0
#  define CU_HAS_BUILTIN_AVAILABLE 0
#endif

#ifdef __has_feature
#  define CU_HAS_FEATURE(feature) __has_feature(feature)
#  define CU_HAS_FEATURE_AVAILABLE 1
#else
#  define CU_HAS_FEATURE(feature) 0
#  define CU_HAS_FEATURE_AVAILABLE 0
#endif

#ifdef __has_extension
#  define CU_HAS_EXTENSION(ext) __has_extension(ext)
#  define CU_HAS_EXTENSION_AVAILABLE 1
#else
#  define CU_HAS_EXTENSION(ext) 0
#  define CU_HAS_EXTENSION_AVAILABLE 0
#endif

#ifdef __has_warning
#  define CU_HAS_WARNING(warning) __has_warning(warning)
#  define CU_HAS_WARNING_AVAILABLE 1
#else
#  define CU_HAS_WARNING(warning) 0
#  define CU_HAS_WARNING_AVAILABLE 0
#endif

#ifdef __FILE__
#  define CU_FILE __FILE__
#  define CU_FILE_AVAILABLE 1
#elif CU_HAS_BUILTIN(__builtin_FILE)
#  define CU_FILE __builtin_FILE()
#  define CU_FILE_AVAILABLE 2
#else
#  define CU_FILE ((const char *)(0))
#  define CU_FILE_AVAILABLE 0
#endif

#ifdef __DATE__
#  define CU_DATE CU_EVAL(__DATE__)
#  define CU_DATE_AVAILABLE 1
#else
#  define CU_DATE ((const char *)(0))
#  define CU_DATE_AVAILABLE 0
#endif

#ifdef __TIME__
#  define CU_TIME CU_EVAL(__TIME__)
#  define CU_TIME_AVAILABLE 1
#else
#  define CU_TIME ((const char *)(0))
#  define CU_TIME_AVAILABLE 0
#endif

#if CU_COMPVER(GNU, 2, 4)
#  define CU_FUNC CU_GNU_EXT __PRETTY_FUNCTION__
#elif CU_LANG_C >= CU_LANG_C99 || CU_COMP_MSVC
#  define CU_FUNC __func__
#else
#  define CU_FUNC ((const char *)(0))
#endif

#ifdef __LINE__
#  define CU_LINE __LINE__
#  define CU_LINE_AVAILABLE 1
#elif CU_HAS_BUILTIN(__builtin_LINE)
#  define CU_LINE __builtin_LINE()
#  define CU_LINE_AVAILABLE 2
#else
#  define CU_LINE 0
#  define CU_LINE_AVAILABLE 0
#endif

#ifdef __COUNTER__
#  define CU_COUNTER __COUNTER__
#  define CU_COUNTER_AVAILABLE 1
#else
#  define CU_COUNTER 0
#  define CU_COUNTER_AVAILABLE 0
#endif

#if CU_COMPVER(MSVC, 15, 0)
#  define CU_PRAGMA(arg) __pragma(arg)
#  define CU_PRAGMA_AVAILABLE 1
#elif CU_COMPVER(GNU, 3, 0) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(SUNPRO, 8, 0) || \
      (CU_COMPVER(IBM, 10, 1) && defined(__C99_PRAGMA_OPERATOR))
#  define CU_PRAGMA(arg) _Pragma(#arg)
#  define CU_PRAGMA_AVAILABLE 1
#else
#  define CU_PRAGMA(arg)
#  define CU_PRAGMA_AVAILABLE 0
#endif

/* ========================= Endianness ======================== */

#if !CU_ARCH_X86
#  if CU_HAS_INCLUDE(<endian.h>)
#    include <endian.h>
#  elif CU_HAS_INCLUDE(<sys/param.h>)
#    include <sys/param.h>
#  endif
#endif

#if defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__) || (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
     __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  define CU_ENDIAN_BIG 1
#  define CU_ENDIAN_LITTLE 0
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || CU_ARCH_X86 || \
      defined(__MIPSEL) || defined(__MIPSEL__) || (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define CU_ENDIAN_BIG 0
#  define CU_ENDIAN_LITTLE 1
#endif

#if !defined(CU_ENDIAN_BIG) && !defined(CU_ENDIAN_LITTLE)
#  define CU_ENDIAN_LITTLE 1
#  define CU_ENDIAN_BIG 0
#endif

#if defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
#  define CU_ENDIAN_FLT_LITTLE 0
#  define CU_ENDIAN_FLT_BIG 1
#elif defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define CU_ENDIAN_FLT_LITTLE 1
#  define CU_ENDIAN_FLT_BIG 0
#else
#  define CU_ENDIAN_FLT_LITTLE CU_ENDIAN_LITTLE
#  define CU_ENDIAN_FLT_BIG CU_ENDIAN_BIG
#endif

#if CU_ENDIAN_LITTLE
#  define CU_UPSHIFT_EQ(val, bits) ((val) <<= (bits))
#  define CU_DWSHIFT_EQ(val, bits) ((val) >>= (bits))
#  define CU_UPSHIFT(val, bits) ((val) << (bits))
#  define CU_DWSHIFT(val, bits) ((val) >> (bits))
#else
#  define CU_UPSHIFT_EQ(val, bits) ((val) >>= (bits))
#  define CU_DWSHIFT_EQ(val, bits) ((val) <<= (bits))
#  define CU_UPSHIFT(val, bits) ((val) >> (bits))
#  define CU_DWSHIFT(val, bits) ((val) << (bits))
#endif

/* ========================= Keywords ======================== */

#if CU_LANG_C >= CU_LANG_C99
#  define CU_INLINE inline
#  define CU_INLINE_AVAILABLE 1
#elif CU_COMP_GNU || CU_COMPVER(ARM, 6, 2)
#  define CU_INLINE __inline__
#  define CU_INLINE_AVAILABLE 1
#elif CU_COMPVER(MSVC, 12, 0) || CU_COMPVER(ARM, 4, 1)
#  define CU_INLINE __inline
#  define CU_INLINE_AVAILABLE 1
#else
#  define CU_INLINE
#  define CU_INLINE_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C99
#  define CU_RESTRICT restrict
#  define CU_RESTRICT_AVAILABLE 1
#elif CU_COMPVER(GNU, 3, 1) || \
      CU_COMPVER(MSVC, 14, 0) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 10, 1) || \
      CU_COMP_CLANG
#  define CU_RESTRICT __restrict
#  define CU_RESTRICT_AVAILABLE 1
#else
#  define CU_RESTRICT
#  define CU_RESTRICT_AVAILABLE 0
#endif

#if CU_COMPVER(GNU, 4, 0) || (CU_COMP_CLANG && !defined(__EMSCRIPTEN__))
#  define CU_OFFSETOF(s,m) (__builtin_offsetof(s,m))
#elif CU_DM_LL
#  define CU_OFFSETOF(s,m) ((unsigned long long)&(((s *)0)->m))
#else
#  define CU_OFFSETOF(s,m)  ((unsigned long)&(((s *)0)->m))
#endif

#if CU_LANG_C >= CU_LANG_C23
#  define CU_ALIGNAS(x) alignas(x)
#  define CU_ALIGNOF(x) alignof(x)
#  define CU_ALIGNMENT_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C11
#  define CU_ALIGNAS(x) _Alignas(x)
#  define CU_ALIGNOF(x) _Alignof(x)
#  define CU_ALIGNMENT_AVAILABLE 1
#else
#  define CU_ALIGNAS(x)
#  define CU_ALIGNOF(x) CU_OFFSETOF(struct{char c;x m;},m)
#  define CU_ALIGNMENT_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C23
#  define CU_CONSTEXPR constexpr
#  define CU_CONSTEXPR_AVAILABLE 1
#else
#  define CU_CONSTEXPR const
#  define CU_CONSTEXPR_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C11
#  define CU_GENERIC _Generic
#  define CU_GENERIC_AVAILABLE 1
#else
#  define CU_GENERIC
#  define CU_GENERIC_AVAILABLE 0
#endif

#if CU_LANG_C
#  define CU_REGISTER register
#  define CU_REGISTER_AVAILABLE 1
#else
#  define CU_REGISTER
#  define CU_REGISTER_AVAILABLE 0
#endif

#if CU_COMPVER(GNU, 2, 0)
#  define CU_THREAD_LOCAL __thread
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_COMP_MSVC
#  define CU_THREAD_LOCAL __declspec(thread)
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C23
#  define CU_THREAD_LOCAL thread_local
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C11
#  define CU_THREAD_LOCAL _Thread_local
#  define CU_THREAD_LOCAL_AVAILABLE 1
#else
#  define CU_THREAD_LOCAL
#  define CU_THREAD_LOCAL_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C11 && !defined(__STDC_NO_ATOMICS__) && CU_HAS_INCLUDE(<stdatomic.h>)
#  define CU_ATOMIC _Atomic
#  define CU_ATOMIC_AVAILABLE 1
#else
#  define CU_ATOMIC
#  define CU_ATOMIC_AVAILABLE 0
#endif

/* ========================= Attributes ======================== */

#if CU_HAS_ATTRIBUTE(__hot__) || CU_COMPVER(GNU, 4, 3)
#  define CU_ATTRIB_COLD __attribute__ ((__cold__))
#  define CU_ATTRIB_HOT __attribute__ ((__hot__))
#else
#  define CU_ATTRIB_COLD
#  define CU_ATTRIB_HOT
#endif

#if CU_HAS_ATTRIBUTE(__flatten__) || CU_COMPVER(GNU, 4, 1)
#  define CU_ATTRIB_FLATTEN __attribute__ ((__flatten__))
#else
#  define CU_ATTRIB_FLATTEN
#endif

#if CU_HAS_ATTRIBUTE(__format__) || CU_COMPVER(GNU, 3, 0)
#  define CU_ATTRIB_FORMAT(fmt) __attribute__ ((__format__ fmt))
#else
#  define CU_ATTRIB_FORMAT(fmt)
#endif

#if CU_HAS_ATTRIBUTE(__nonnull__) || CU_COMPVER(GNU, 3, 3)
#  define CU_ATTRIB_NONNULL(inds) __attribute__ ((__nonnull__ inds))
#else
#  define CU_ATTRIB_NONNULL(inds)
#endif

#if CU_HAS_ATTRIBUTE(__nonnull__) || CU_COMPVER(GNU, 3, 1)
#  define CU_ATTRIB_USED __attribute__ ((__used__))
#else
#  define CU_ATTRIB_USED
#endif

#if CU_COMPVER(MSVC, 14, 0)
#  define CU_DEPRECATED(msg) __declspec(deprecated(msg))
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_COMPVER(MSVC, 13, 10)
#  define CU_DEPRECATED(msg) __declspec(deprecated)
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_HAS_EXTENSION(attribute_deprecated_with_message) || \
       CU_COMPVER(GNU, 4, 5) || \
       CU_COMPVER(INTEL, 13, 0) || \
       CU_COMPVER(ARM, 5, 6) || \
       CU_COMPVER(SUNPRO, 5, 13)
#  define CU_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_HAS_ATTRIBUTE(deprecated) || CU_COMPVER(GNU, 3, 1) || CU_COMPVER(ARM, 4, 1)
#  define CU_DEPRECATED(msg) __attribute__((__deprecated__))
#  define CU_DEPRECATED_AVAILABLE 1
#else
#  define CU_DEPRECATED(msg)
#  define CU_DEPRECATED_AVAILABLE 0
#endif

#if CU_HAS_ATTRIBUTE(warn_unused_result) || CU_COMPVER(GNU, 3, 4) ||  CU_COMPVER(INTEL, 13, 0)
#  define CU_ATTRIB_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#elif defined(_Check_return_)
#  define CU_ATTRIB_WARN_UNUSED_RESULT _Check_return_
#else
#  define CU_ATTRIB_WARN_UNUSED_RESULT
#endif

#if CU_LANG_C >= CU_LANG_C11
#  define CU_ATTRIB_NORETURN _Noreturn
#elif CU_HAS_ATTRIBUTE(noreturn) || \
      CU_COMPVER(GNU, 3, 2) || \
      CU_COMPVER(SUNPRO, 5, 11) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 10, 1)
#  define CU_ATTRIB_NORETURN __attribute__((__noreturn__))
#elif CU_COMPVER(SUNPRO, 5, 10)
#  define CU_ATTRIB_NORETURN _Pragma("does_not_return")
#elif CU_COMPVER(MSVC, 13, 10)
#  define CU_ATTRIB_NORETURN __declspec(noreturn)
#else
#  define CU_ATTRIB_NORETURN
#endif

#if CU_HAS_ATTRIBUTE(pure) || \
    CU_COMPVER(GNU, 2, 9) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1)
#  define CU_ATTRIB_PURE __attribute__((__pure__))
#elif CU_COMPVER(SUNPRO, 5, 10)
#  define CU_ATTRIB_PURE _Pragma("does_not_write_global_data")
#else
#  define CU_ATTRIB_PURE
#endif

#if CU_HAS_ATTRIBUTE(const) || \
    CU_COMPVER(GNU, 2, 5) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1)
#  define CU_ATTRIB_CONST __attribute__((__const__))
#elif CU_COMPVER(SUNPRO, 5, 10)
#  define CU_ATTRIB_CONST _Pragma("no_side_effect")
#else
#  define CU_ATTRIB_CONST CU_ATTRIB_PURE
#endif

#if CU_HAS_ATTRIBUTE(always_inline) || \
    CU_COMPVER(GNU, 4, 0) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1)
#  define CU_ATTRIB_ALWAYSINLINE __attribute__((__always_inline__)) CU_INLINE
#elif CU_COMPVER(MSVC, 12, 0)
#  define CU_ATTRIB_ALWAYSINLINE __forceinline
#else
#  define CU_ATTRIB_ALWAYSINLINE CU_INLINE
#endif

#if CU_HAS_ATTRIBUTE(noinline) || \
    CU_COMPVER(GNU, 4, 0) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1)
#  define CU_ATTRIB_NEVERINLINE __attribute__((__noinline__))
#elif CU_COMPVER(MSVC, 13, 0)
#  define CU_ATTRIB_NEVERINLINE __declspec(noinline)
#else
#  define CU_ATTRIB_NEVERINLINE
#endif

#if CU_HAS_ATTRIBUTE(nothrow) || CU_COMPVER(GNU, 3, 3) || CU_COMPVER(INTEL, 13, 0)
#  define CU_ATTRIB_NOTHROW __attribute__((__nothrow__))
#else
#  define CU_ATTRIB_NOTHROW
#endif

#if CU_HAS_ATTRIBUTE(returns_nonnull) || CU_COMPVER(GNU, 4, 9)
#  define CU_ATTRIB_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#elif defined(_Ret_notnull_)
#  define CU_ATTRIB_RETURNS_NONNULL _Ret_notnull_
#else
#  define CU_ATTRIB_RETURNS_NONNULL
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#  define CU_VISIBILITY_PRIVATE
#  define CU_VISIBILITY_PUBLIC __declspec(dllexport)
#  define CU_VISIBILITY_IMPORT __declspec(dllimport)
#elif CU_HAS_ATTRIBUTE(visibility) || \
      CU_COMPVER(GNU, 3, 3) || \
      CU_COMPVER(SUNPRO, 5, 11) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 13, 1)
#  define CU_VISIBILITY_PRIVATE __attribute__((__visibility__("hidden")))
#  define CU_VISIBILITY_PUBLIC __attribute__((__visibility__("default")))
#  define CU_VISIBILITY_IMPORT extern
#else
#  define CU_VISIBILITY_PRIVATE
#  define CU_VISIBILITY_PUBLIC
#  define CU_VISIBILITY_IMPORT extern
#endif

#if CU_COMPVER(CLANG, 12, 0) || CU_COMPVER(GNU, 7, 0) || (CU_HAS_ATTRIBUTE(__fallthrough__) && !CU_COMP_CLANG)
#  define CU_FALLTHROUGH  ((void)(0)); __attribute__ ((__fallthrough__));
#  define CU_FALLTHROUGH_AVAILABLE 1
#elif defined(__fallthrough)
#  define CU_FALLTHROUGH __fallthrough
#  define CU_FALLTHROUGH_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C23
#  define CU_FALLTHROUGH [[__fallthrough__]]
#  define CU_FALLTHROUGH_AVAILABLE 1
#else
#  define CU_FALLTHROUGH ((void)(0))
#  define CU_FALLTHROUGH_AVAILABLE 0
#endif

/* ========================= Debugging ======================== */

#if (defined(NDEBUG) || (defined(_DEBUG) && !(_DEBUG - 0))) && !CU_SETTING_FORCE_DEBUG
#  define CU_RELEASE 1
#  define CU_DEBUG 0
#else
#  define CU_RELEASE 0
#  define CU_DEBUG 1
#endif

#ifdef __STDC_HOSTED__
#  if (__STDC_HOSTED__ - 0) == 1
#    define CU_FREESTANDING 0
#  else
#    define CU_FREESTANDING 1
#  endif
#else
#  define CU_FREESTANDING 1
#endif

#if CU_HAS_WARNING("-Wunknown-pragmas") || CU_COMPVER(GNU, 4, 3)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")
#elif CU_COMPVER(INTEL, 13, 0)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("warning(disable:161)")
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:4068))
#else
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
#endif

#if CU_HAS_WARNING("-Wpre-c11-compat")
#  define CU_DIAGNOSTICS_DISABLE_PREC11 _Pragma("GCC diagnostic ignored \"-Wpre-c11-compat\"")
#else
#  define CU_DIAGNOSTICS_DISABLE_PREC11
#endif

#if CU_HAS_WARNING("-Wreserved-macro-identifier") && CU_HAS_WARNING("-Wreserved-identifier")
#  define CU_DIAGNOSTICS_DISABLE_RESERVED \
_Pragma("GCC diagnostic ignored \"-Wreserved-macro-identifier\"") \
_Pragma("GCC diagnostic ignored \"-Wreserved-identifier\"") \
_Pragma("GCC diagnostic ignored \"-Wreserved-id-macro\"")
#elif CU_HAS_WARNING("-Wreserved-id-macro")
#  define CU_DIAGNOSTICS_DISABLE_RESERVED _Pragma("GCC diagnostic ignored \"-Wreserved-id-macro\"")
#else
#  define CU_DIAGNOSTICS_DISABLE_RESERVED
#endif

#if CU_HAS_WARNING("-Wdisabled-macro-expansion")
#  define CU_DIAGNOSTICS_DISABLE_MACRO_EXP _Pragma("GCC diagnostic ignored \"-Wdisabled-macro-expansion\"")
#else
#  define CU_DIAGNOSTICS_DISABLE_MACRO_EXP
#endif

#if CU_COMP_CLANG || CU_COMPVER(GNU, 4, 6)
#  define CU_DIAGNOSTICS_PUSH _Pragma("GCC diagnostic push")
#  define CU_DIAGNOSTICS_POP _Pragma("GCC diagnostic pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(INTEL, 13, 0)
#  define CU_DIAGNOSTICS_PUSH _Pragma("warning(push)")
#  define CU_DIAGNOSTICS_POP _Pragma("warning(pop)")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_DIAGNOSTICS_PUSH __pragma(warning(push))
#  define CU_DIAGNOSTICS_POP __pragma(warning(pop))
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(ARM, 5, 6)
#  define CU_DIAGNOSTICS_PUSH _Pragma("push")
#  define CU_DIAGNOSTICS_POP _Pragma("pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#else
#  define CU_DIAGNOSTICS_PUSH
#  define CU_DIAGNOSTICS_POP
#  define CU_DIAGNOSTICS_AVAILABLE 0
#endif

#if CU_HAS_WARNING("-Wunknown-pragmas")
#  define CU_MESSAGE(msg) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS \
CU_PRAGMA(message msg) \
CU_DIAGNOSTICS_POP
#elif CU_COMPVER(GNU, 4, 4) || CU_COMPVER(INTEL, 13, 0)
#  define CU_MESSAGE(msg) CU_PRAGMA(message msg)
#else
#  define CU_MESSAGE(msg)
#endif

#if CU_HAS_WARNING("-Wunknown-pragmas")
#  define CU_WARNING(msg) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS \
CU_PRAGMA(GCC warning msg) \
CU_DIAGNOSTICS_POP
#elif CU_COMPVER(GNU, 4, 8) || CU_COMPVER(INTEL, 13, 0)
#  define CU_WARNING(msg) CU_PRAGMA(GCC warning msg)
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_WARNING(msg) CU_PRAGMA(message(msg))
#else
#  define CU_WARNING(msg) CU_MESSAGE(msg)
#endif

#if CU_FREESTANDING && CU_HAS_BUILTIN(__builtin_trap)
#  define CU_BREAKPOINT() __builtin_trap()
#elif defined(__ibmxl__) || defined(__xlC__)
#  include <builtins.h>
#  define CU_BREAKPOINT() __trap(42)
#elif CU_COMP_MSVC || CU_HAS_BUILTIN(__debugbreak) || CU_COMP_INTEL
#  define CU_BREAKPOINT() __debugbreak()
#elif CU_HAS_BUILTIN(__builtin_debugtrap) || (CU_ARCH_ARM_AARCH64 && CU_OS_MAC)
#  define CU_BREAKPOINT() __builtin_debugtrap()
#elif defined(__ARMCC_VERSION)
#  define CU_BREAKPOINT() __breakpoint(42)
#elif defined(__DMC__) && defined(_M_IX86)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM int 3h; }
#elif CU_ARCH_X86
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__("int3"); }
#elif CU_ARCH_ARM_THUMB
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__(".inst 0xde01"); }
#elif CU_ARCH_ARM_AARCH64
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__(".inst 0xd4200000"); }
#elif CU_ARCH_ARM
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__(".inst 0xe7f001f0"); }
#elif defined(__alpha__) && !defined(__osf__)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__("bpt"); }
#elif defined(_54_)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__("ESTOP"); }
#elif defined(_55_)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
#elif defined(_64P_)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__("SWBP 0"); }
#elif defined(_6x_)
CU_ATTRIB_USED static void __cu_breakpoint(void) { CU_ASM __volatile__("NOP\n .word 0x10000000"); }
#elif CU_OS_UNIX
#  include <signal.h>
CU_ATTRIB_USED static void __cu_breakpoint(void)
{
#ifdef SIGTRAP
	raise(SIGTRAP);
#else
	raise(SIGABRT);
#endif
}
#else
CU_ATTRIB_USED static void __cu_breakpoint(void) { *(volatile char *)0 = 0; }
#endif

#ifndef CU_BREAKPOINT
#  define CU_BREAKPOINT() __cu_breakpoint()
#endif

#if CU_HAS_BUILTIN(__builtin_trap)
#  define CU_TRAP() __builtin_trap()
#elif CU_HAS_INCLUDE(<stdlib.h>)
#  include <stdlib.h>
#  define CU_TRAP() abort()
#endif

#ifndef CU_TRAP
#  if CU_OS_UNIX
#    include <signal.h>
CU_ATTRIB_USED CU_ATTRIB_NORETURN static void __cu_trap(void) { signal(SIGABRT, SIG_DFL); while (1) raise(SIGABRT); }
#  else
CU_ATTRIB_USED CU_ATTRIB_NORETURN static void __cu_trap(void) { while (1) *(volatile char *)0 = 0; }
#  endif
#  define CU_TRAP() __cu_trap()
#endif

#if CU_LANG_C >= CU_LANG_C23
#  define CU_STATIC_ASSERT(cond, msg) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_PREC11 \
static_assert(cond, msg); \
CU_DIAGNOSTICS_POP
#elif CU_LANG_C >= CU_LANG_C11
#  define CU_STATIC_ASSERT(cond, msg) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_PREC11 \
_Static_assert(cond, msg); \
CU_DIAGNOSTICS_POP
#elif CU_COMP_MSVC
#  define CU_STATIC_ASSERT(cond, msg) typedef char CU_CONCAT(cu_static_assert_line, CU_EVAL(CU_LINE))[!!(cond) ? 1 : -1];
#else
#  define CU_STATIC_ASSERT(cond, msg) enum { CU_CONCAT(cu_static_assert_line, CU_EVAL(CU_LINE)) = 1 / (!!(cond)) };
#endif

#if CU_RELEASE
#  define CU_ASSERT(cond)
#elif CU_HAS_INCLUDE(<assert.h>)
#  include <assert.h>
#  define CU_ASSERT(cond) assert(cond)
#elif CU_HAS_INCLUDE(<stdlib.h>) && CU_HAS_INCLUDE(<stdio.h>)
#  include <stdio.h>
#  include <stdlib.h>
CU_ATTRIB_NORETURN CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2,3,4)) CU_ATTRIB_USED CU_ATTRIB_COLD
static void __cu_assert_fail(int line, const char *func, const char *file, const char *cond)
{
	printf("%s:%d: %s: Assertion '%s' failed.\n", file, line, func, cond);
	abort();
}
#  define CU_ASSERT(cond) do { if (CU_UNLIKELY(!(cond))) __cu_assert_fail(CU_LINE, CU_FUNC, CU_FILE, CU_STRINGIFY(cond)); } while (0)
#else
#  define CU_ASSERT(cond) do { if (!(CU_LIKELY((cond)))) { CU_BREAKPOINT(); CU_TRAP(); } } while (0)
#endif

#define CU_UNUSED(var) ((void)(var))
#define CU_EMPTY() ((void)(0))

/* ========================= Types ======================== */

#if CU_SETTING_TYPES

typedef signed char i8;
typedef signed short i16;
typedef unsigned char u8;
typedef unsigned short u16;

#if CU_HAS_INCLUDE(<stdint.h>) || CU_COMPVER(GNU, 4, 5)
#  include <stdint.h>
   typedef int32_t i32;
   typedef int64_t i64;
   typedef uint32_t u32;
   typedef uint64_t u64;
#elif CU_DM_LL
   typedef signed int i32;
   typedef signed long long int i64;
   typedef unsigned int u32;
   typedef unsigned long long int u64;
#else
   typedef signed int i32;
   typedef signed long int i64;
   typedef unsigned int u32;
   typedef unsigned long int u64;
#endif

#if CU_DM_64BIT
   typedef i64 iptr;
   typedef u64 uptr;
#else
   typedef i32 iptr;
   typedef u32 uptr;
#endif

typedef i64 imax;
typedef u64 umax;

#if defined(INT128_MAX) || defined(__SIZEOF_INT128__)
   CU_GNU_EXT typedef __int128 imaxl;
   CU_GNU_EXT typedef unsigned __int128 umaxl;
#  define CU_INT128_AVAILABLE 1
#else
   typedef imax imaxl;
   typedef umax umaxl;
#  define CU_INT128_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C23 && defined(__STDC_IEC_60559_DFP__)
   typedef _Decimal32 real32;
   typedef _Decimal64 real64;
   typedef _Decimal128 real128;
#  define CU_DECIMAL128_AVAILABLE 1
#else
   typedef float real32;
   typedef double real64;
   typedef long double real128;
#  if CU_DM_64BIT && (CU_OS_HPUX || CU_ARCH_SPARC || CU_ARCH_MIPS || CU_ARCH_ARM)
#    define CU_DECIMAL128_AVAILABLE 1
#  else
#    define CU_DECIMAL128_AVAILABLE 0
#  endif
#endif

#endif

#if CU_DM_LL
#  define CU_U64_C(a) a ## ULL
#  define CU_I64_C(a) a ## LL
#  define CU_U64_FMT "llu"
#  define CU_I64_FMT "lld"
#else
#  define CU_U64_C(a) a ## UL
#  define CU_I64_C(a) a ## L
#  define CU_U64_FMT "lu"
#  define CU_I64_FMT "ld"
#endif

#if CU_DM_64BIT
#  define CU_UPTR_C(a) CU_U64_C(a)
#  define CU_UPTR_FMT CU_U64_FMT
#  define CU_IPTR_C(a) CU_I64_C(a)
#  define CU_IPTR_FMT CU_I64_FMT
#else
#  define CU_UPTR_C(a) a
#  define CU_UPTR_FMT "u"
#  define CU_IPTR_C(a) a
#  define CU_IPTR_FMT "d"
#endif

/* ========================= Optimization ======================== */

#if CU_PRAGMA_AVAILABLE && CU_COMPVER(GNU, 3, 0)
#  define CU_UNROLL(n) CU_PRAGMA("GCC unroll " #n)
#else
#  define CU_UNROLL(n)
#endif

#if CU_COMPVER(MSVC, 13, 10) || CU_COMPVER(INTEL, 13, 0)
#  define CU_ASSUME(expr) __assume(expr)
#elif CU_HAS_BUILTIN(__builtin_assume)
#  define CU_ASSUME(expr) __builtin_assume(expr)
#endif

#if (CU_HAS_BUILTIN(__builtin_unreachable) && !CU_COMP_ARM) || \
     CU_COMPVER(GNU, 4, 5) || \
     CU_COMPVER(INTEL, 13, 0) || \
     CU_COMPVERP(IBM, 13, 1, 5)
#  define CU_UNREACHABLE() __builtin_unreachable()
#elif defined(CU_ASSUME)
#  define CU_UNREACHABLE() CU_ASSUME(0)
#else
#  define CU_UNREACHABLE()
#endif
#ifndef CU_ASSUME
#  define CU_ASSUME(expr) ((void)((expr) ? 1 : (CU_UNREACHABLE(),1)))
#endif

#if (CU_HAS_BUILTIN(__builtin_expect_with_probability)) || CU_COMPVER(GNU, 9, 0)
#  define CU_PREDICT(expr, result, probability) __builtin_expect_with_probability((expr), (result), (probability))
#  define CU_LIKELY(expr) __builtin_expect(!!(expr), 1)
#  define CU_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#elif CU_HAS_BUILTIN(__builtin_expect) || \
      CU_COMPVER(GNU, 3, 0) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 10, 1)
#  define CU_PREDICT(expr, result, probability) (((probability > 0.5) ? __builtin_expect((expr), (result)) : ((void)((result)), (expr))))
#  define CU_LIKELY(expr) __builtin_expect(!!(expr), 1)
#  define CU_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#  define CU_PREDICT(expr, result, probability) ((void)((result)), (expr))
#  define CU_LIKELY(expr) (!!(expr))
#  define CU_UNLIKELY(expr) (!!(expr))
#endif

#if CU_HAS_BUILTIN(__builtin_unpredictable)
#  define CU_UNPREDICTABLE(expr) __builtin_unpredictable((expr))
#else
#  define CU_UNPREDICTABLE(expr) CU_PREDICT(expr, 1, 0.5)
#endif

/* ========================= Constants ======================== */

#define CU_I8MAX 0x7F
#define CU_U8MAX 0xFF
#define CU_I16MAX 0x7FFF
#define CU_U16MAX 0xFFFF
#define CU_I32MAX 0x7FFFFFFF
#define CU_U32MAX 0xFFFFFFFF
#define CU_U64MAX ((u64)(-1))
#define CU_I64MAX ((i64)CU_DWSHIFT(CU_U64MAX, 1))

#if CU_DM_LL
#  define CU_INTMAX CU_I32MAX
#  define CU_UINTMAX CU_U32MAX
#  define CU_LONGMAX CU_I32MAX
#  define CU_ULONGMAX CU_U32MAX
#  define CU_LLONGMAX CU_I64MAX
#  define CU_ULLONGMAX CU_U64MAX
#else
#  define CU_INTMAX CU_I32MAX
#  define CU_UINTMAX CU_U32MAX
#  define CU_LONGMAX CU_I64MAX
#  define CU_ULONGMAX CU_U64MAX
#  define CU_LLONGMAX CU_I64MAX
#  define CU_ULLONGMAX CU_U64MAX
#endif

#if CU_DM_64BIT
#  define CU_UPTRMAX CU_U64MAX
#  define CU_IPTRMAX CU_I64MAX
#else
#  define CU_UPTRMAX CU_U32MAX
#  define CU_IPTRMAX CU_I32MAX
#endif

#ifdef PATH_MAX
#  define CU_PATH_MAX PATH_MAX
#elif defined (MAX_PATH)
#  define CU_PATH_MAX MAX_PATH
#elif CU_OS_WINDOWS
#  define CU_PATH_MAX 520
#elif CU_OS_MAC || CU_OS_BSD
#  define CU_PATH_MAX 1024
#elif CU_OS_UNIX
#  define CU_PATH_MAX 4096
#else
#  define CU_PATH_MAX 512
#endif

#if CU_OS_WINDOWS
#  define CU_FILE_SEPARATOR '\\'
#  define CU_FILE_SEPARATOR_DQ "\\"
#else
#  define CU_FILE_SEPARATOR '/'
#  define CU_FILE_SEPARATOR_DQ "/"
#endif

#ifndef NULL
#  define NULL ((void *)(0))
#endif

/* ========================= Functions ======================== */

#if CU_SETTING_FUNCS

#if CU_SETTING_FILE_FUNCS && !CU_OS_WINDOWS && (!CU_HAS_INCLUDE(<sys/stat.h>) || !CU_HAS_INCLUDE(<dirent.h>))
#  if !CU_SETTING_QUIET
CU_WARNING("File functions are unavailable.")
#  endif
#  undef CU_SETTING_FILE_FUNCS
#  define CU_SETTING_FILE_FUNCS 0
#endif

#if CU_SETTING_NETWORK_FUNCS && !CU_OS_WINDOWS && !CU_OS_UNIX
#  if !CU_SETTING_QUIET
CU_WARNING("Networking functions are unavailable.")
#  endif
#  undef CU_SETTING_NETWORK_FUNCS
#  define CU_SETTING_NETWORK_FUNCS 0
#endif

#if CU_SETTING_THREAD_FUNCS && !(CU_THREAD_POSIX_AVAILABLE || CU_THREAD_C_AVAILABLE || CU_THREAD_WIN_AVAILABLE)
#  if !CU_SETTING_QUIET
CU_WARNING("Threading functions are unavailable.")
#  endif
#  undef CU_SETTING_THREAD_FUNCS
#  define CU_SETTING_THREAD_FUNCS 0
#endif

#endif

#if CU_SETTING_FUNCS

#define A_NNULL(p) CU_ATTRIB_NONNULL(p)
#define A_NTH CU_ATTRIB_NOTHROW
#define A_WUR CU_ATTRIB_WARN_UNUSED_RESULT
#define A_NTL(p) CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL(p)
#define A_RES CU_RESTRICT

/* ========================= String ======================== */

#if CU_SETTING_STRING_FUNCS

typedef struct custr
{
	char *str;
	uptr len, cap;
} custr;

#define CUSTR_EMPTY { NULL, 0, 0 }

/* Creates a custr from a normal string.
   The given custr pointer should be uninitialized, empty or (if it contains allocated text) cleared with custr_clear. */
A_NTL((1,2)) int custr_create(custr *A_RES c, const char *A_RES str);
/* Creates a custr using an already allocated string and returns the given custr.
   You can provide the allocated size or 0 to assume it is strlen + 1.
   The given custr pointer should be uninitialized, empty or (if it contains allocated text) cleared with custr_clear.
   The given allocated string should not be modified directly afterwards. */
A_NTL((1,2)) custr *custr_allocd(custr *A_RES c, char *A_RES allocdstr, uptr allocd_bytes);

/* Changes reserved space of custr and returns whether it succeeded.
   Setting the reserved space lower than or equal to the string length will truncate it and will always succeed.
   0 bytes deallocates it and will always succeed. */
A_NTL((1)) int custr_reserve(custr *c, uptr bytes);
/* Copies a custr to another. */
A_NTL((1,2)) int custr_copy(const custr *A_RES copy, custr *A_RES paste);

/* Sets custr length and moves terminator.
   No effect if given length is is larger than or equal to the string length. */
A_NTL((1)) int custr_shrinkto(custr *c, uptr shrinked_len);
/* Deallocates a custr's data and returns it. */
A_NTL((1)) custr *custr_clear(custr *c);
/* Optimizes the capacity of the given custr. */
A_NTL((1)) int custr_optimize(custr *c);

/* Inserts 'to_insert' into 'c' at their respective offsets.
   Fails if c_offset is larger than c's length or on allocation failure.
   Does nothing successfully if to_insert is empty (i.e. only a terminator). */
A_NTL((1,3)) int custr_insert(custr *A_RES c, uptr c_offset, const char *A_RES to_insert);
/* Same as custr_insert but 'c_offset' is the length of 'c'. */
A_NTL((1,2)) int custr_append(custr *A_RES c, const char *A_RES to_append);

/* Gets a substring of a given custr (inclusive indices). The ending index is clamped to the last character.
   The substring should not be allocated beforehand. */
A_NTL((1,2)) int custr_sub(const custr *A_RES c, custr *A_RES subresult, uptr start_ind, uptr end_ind);
/* Sets the given custr to a substring of itself.
   Both indices are inclusive.
   The ending index is clamped to the last character.
   Fails if start_ind is larget than end_ind or if start_ind is larger than or equal to the string length. */
A_NTL((1)) int custr_tosub(custr *c, uptr start_ind, uptr end_ind);
/* Removes a specified section of the given custr.
   count is clamped if it would go past the end.
   No effect if starting index is past the end of the string. */
A_NTL((1)) void custr_cut(custr *c, uptr start_ind, uptr count);

/* Returns the number of occurrences of a character in a custr.
   The null terminator at the end of the string is excluded from counting. */
A_NTL((1)) int custr_count(const custr *c, char target);
/* Returns the number of occurrences of a substring in a custr.
   The null terminator at the end of the string is excluded from counting. */
A_NTL((1,2)) int custr_countsub(const custr *A_RES c, const char *A_RES target);

/* Sets the given custr to describe a variadically formatted string.
   The given custr pointer should be uninitialized, empty or (if it contains allocated text) cleared with custr_clear.
   Support for va_copy and snprintf is required. */
A_NTL((1,2)) CU_ATTRIB_FORMAT((printf, 2, 3)) int custr_fmt(custr *A_RES c, char *A_RES fmt, ...);

/* Appends the given file or directory name to 'c' (assuming it is a path).
   If given name is NULL or "..", 'c' will be changed to describe the parent directory instead. */
A_NTL((1)) int custr_cd(custr *A_RES c, const char *A_RES name);
/* Simplify the given path, assuming 'c' describes one. */
A_NTL((1)) void custr_simplify(custr *c);

/* Returns the index of the nth occurrence of a character in a custr, or CU_UPTRMAX if not found.
   The null terminator at the end of the string is not included in the search.
   The offset determines where to start the search. If it is larger than or equal to the string length, CU_UPTRMAX is returned.
   n =  0,  1 returns the first and second occurrence respectively.
   n = -1, -2 returns the last and second last occurrence respectively.
   If searching backwards (negative n), the offset will also apply backwards. */
A_NTL((1)) uptr custr_find(const custr *c, uptr c_offset, char target, int n);
/* Same as custr_find, but searches for any character that is NOT target. */
A_NTL((1)) uptr custr_findnot(const custr *c, uptr c_offset, char target, int n);

/* Same as custr_find, but for substrings instead of single characters. */
A_NTL((1,3)) uptr custr_findsub(const custr *A_RES c, uptr c_offset, const char *A_RES target_substr, int n);

/* Replaces a character with another. Null terminator replacement removes all occurrences.
   No effect if target character is a null terminator. */
A_NTL((1)) void custr_replace(custr *c, uptr c_offset, char target, char replacement);

/* Replaces a substring with another. A replacement of a null pointer or null custr removes all occurrences.
   No effect if target substring is empty. */
A_NTL((1,3)) int custr_replacesub(custr *A_RES c, uptr c_offset, const char *A_RES target, const char *A_RES replacement);

#endif

/* ========================= Containers ======================== */

/* Dynamic array object. */
typedef struct cu_list
{
	void *data;
	uptr len, cap, elem;
} cu_list;

#define CU_LIST_INIT(sizeof_element) { NULL, 0, 0, sizeof_element }

/* Initializes and returns the given cu_list value. You can also use the static initializer. */
A_NTL((1)) cu_list *cu_list_init(cu_list *l, uptr sizeof_element);
/* Reserves the given number of elements int the given cu_list. Returns 1 on success, 0 on failure.
   No effect if it is smaller than current capacity and returns 1. */
A_NTL((1)) int cu_list_reserve(cu_list *l, uptr cap);
/* Deallocates and returns the given cu_list value. The element size is retained. */
A_NTL((1)) cu_list *cu_list_clear(cu_list *l);

/* Returns the element at the given index in the cu_list. */
A_NTL((1)) void *cu_list_at(cu_list *l, uptr ind);
/* Returns the element at the given index in the cu_list.
   Returns NULL if the index is out-of-bounds. */
A_NTL((1)) void *cu_list_atsf(cu_list *l, uptr ind);
/* Returns whether any item in the array matches the given item. */
A_NTL((1,2)) int cu_list_has(cu_list *A_RES l, void *A_RES tomatch);

/* Inserts a number of elements to the cu_list at the specified index. Returns 1 on success, 0 on failure.
   Also fails if index is past the end of the cu_list (larger than its length). */
A_NTL((1,2)) int cu_list_insert(cu_list *A_RES l, void *A_RES elems, uptr nelems, uptr ind);
/* Appends the given element to the end of the list. Returns 1 on success, 0 on failure. */
A_NTL((1,2)) int cu_list_add(cu_list *A_RES l, void *A_RES elem);

/* Remove a specified number of elements from the cu_list starting from the given index.
   No effect if index is past the end. Count is clamped to the last element. */
A_NTL((1)) void cu_list_cut(cu_list *l, uptr ind, uptr count);

/* Function pointer for hash map object that returns a hash value from a given key. */
typedef uptr (*cu_hmap_hashfunc)(const void *key);
/* Function pointer for hash map object that returns whether two keys are equivalent. */
typedef int (*cu_hmap_equalfunc)(const void *key_a, const void *key_b);

/* Inner hash map object storing key-value pairs as a linked list. */
typedef struct cu_hmap_element
{
	/* Given key that corresponds to the data. */
	void *key;
	/* Actual data associated with the key. */
	void *data;
	/* Next linked element, or NULL if this is the last element.
	   Do not modify this. */
	struct cu_hmap_element *next;
} cu_hmap_element;

#define CU_HMAP_NBUCKETS 256

/* Hash map object. */
typedef struct cu_hmap
{
	cu_hmap_element *buckets[CU_HMAP_NBUCKETS];
	cu_hmap_equalfunc equalfunc;
	cu_hmap_hashfunc hashfunc;
} cu_hmap;

/* Initializes and returns the given cu_hmap value. */
A_NTL((1)) cu_hmap *cu_hmap_init(cu_hmap *h, cu_hmap_equalfunc equalfunc, cu_hmap_hashfunc hashfunc);
/* Deallocates and returns the given cu_hmap value. The equivalence and hash function pointers are retained.
   You can optionally specify to also deallocate the keys and/or data. */
A_NTL((1)) cu_hmap *cu_hmap_clear(cu_hmap *A_RES h, int free_keys, int free_data);

/* Add a key-value pair to the given cu_hmap. Returns 1 on success, 0 on failure. */
A_NTL((1,2)) int cu_hmap_add(cu_hmap *A_RES h, void *data, void *key);
/* Find and return the data associated with the key, or NULL if not found. */
A_NTL((1)) void *cu_hmap_find(cu_hmap *A_RES h, const void *A_RES key);
/* Remove a key-value pair from the given cu_hmap.
   The associated data is returned and you can optionally specify to free the internally stored key.
   If it is not found however, NULL is returned with no other effect. */
A_NTL((1)) void *cu_hmap_remove(cu_hmap *A_RES h, const void *A_RES key, int free_key);

/* Destructively adds all key-value data from hash map 'b' into hash map 'a'.
   This directly uses pointers from hash map 'b' for performance, so to
   prevent confusion from containing references to another hash map,
   all pointers in 'b' are cleared (this does not deallocate them). */
A_NTL((1,2)) void cu_hmap_combine(cu_hmap *A_RES a, cu_hmap *A_RES b);

/* Iterate through all elements in the hash map, running the given function pointer with the key-value pairs.
   The 'user' pointer will simply be passed to the function as the 'user' argument. */
A_NTL((1,3)) cu_hmap *cu_hmap_iterate(cu_hmap *h, void *user, void (*work_func)(const void *key, void *data, void *user));

/* ========================= Filesystem ======================== */

#if CU_SETTING_FILE_FUNCS

typedef struct cu_file_info
{
	u64 fsize_bytes; /* Size of the file in bytes. */
	i64 mod_time;    /* Last modification time. */
	i64 access_time; /* Last access time. */
	i64 create_time; /* Creation time. */
} cu_file_info;

/* Returns whether the path is a file. */
A_NTL((1)) int cu_file_exists(const char *path);
/* Returns whether the path is a directory. */
A_NTL((1)) int cu_dir_exists(const char *path);

/* Creates a directory. */
A_NTL((1)) int cu_dir_create(const char *path);

/* Returns the name of each filesystem object (files, nested directories, links, etc) in a given directory.
   If 'fullname' is non-zero, the object's name is prefixed with the given directory path, otherwise, only the object's name is stored.
   The 'count' pointer will store how many objects there are.
   Returns NULL on allocation error or if the directory is inaccessible/does not exist. */
A_NTL((1,3)) char **cu_dir_list(const char *path, int fullname, int *count);

/* Deallocate a directory list. */
A_NTL((1)) void cu_dir_close(char **dirlist, int count);

/* Read a number of bytes from a file.
   'bytes' determines how many bytes of the given file are read into 'result'.
   It is clamped to the file's size. Afterwards, 'bytes' is changed to the file size.
   If 'bytes' is a NULL pointer or zero, the entire file is read into 'result'.
   If 'result' is NULL, enough data is allocated to store the file contents.
   If the file is not a binary file, the contents are null terminated.
   Returns the given 'result' pointer or allocated data; otherwise NULL on error. */
A_NTL((1)) void *cu_file_read(const char *A_RES path, void *A_RES result, int binary_file, uptr *A_RES bytes);

/* Write or append a number of bytes of 'content' into a file.
   Valid modes can be found in the macros CU_FILE_WRITETXT, CU_FILE_WRITEBIN, CU_FILE_APPENDTXT and CU_FILE_APPENDBIN.
   Returns 0 on error, which can be from failing to write to/open the file or an invalid mode value. */
A_NTL((1,2)) int cu_file_write(const char *A_RES path, const void *A_RES content, unsigned int mode, uptr bytes);

#define CU_FILE_WRITETXT 0
#define CU_FILE_WRITEBIN 1
#define CU_FILE_APPENDTXT 2
#define CU_FILE_APPENDBIN 3

/* Gets file information. Returns 0 if the file does not exist. */
A_NTL((1,2)) int cu_file_getinfo(const char *A_RES path, cu_file_info *A_RES f_info);

/* Determines the current running executable's path.
   Returns allocated string and changes 'len' to allocated bytes.
   On error, NULL is returned and passed length pointer is unaffected. */
A_NTH char *cu_file_exe_path(const char *A_RES argv_first, uptr *A_RES allocated);

/* Removes the file at the given path. */
A_NTL((1)) int cu_file_delete(const char *path);
/* Removes the directory at the given path.
   Fails if the directory is not empty unless 'rm_contents' is non-zero. */
A_NTL((1)) int cu_dir_delete(const char *path, int rm_contents);

#endif

/* ========================= Resources ======================== */

#if CU_SETTING_RESOURCES_FUNCS

/* Memory usage and availability information. */
typedef struct cu_res_mem
{
	u64 physical_present; /* Amount of physical memory present on the system. */
	u64 physical_free;    /* Amount of free physical memory. */
	u64 physical_used;    /* Total physical memory used by this process. */

	u64 virtual_present;  /* Amount of virtual memory present on the system. */
	u64 virtual_free;     /* Amount of free virtual memory. */
	u64 virtual_used;     /* Total virtual memory used by this process. */
} cu_res_mem;

struct cu_res_cpu_cache
{
	/* Cache line size in bytes. */
	u32 line;
	/* Ways of associativity. If it is fully associative, it will be -1.
	   Like with all other values, it will be 0 if it was unable to be determined. */
	i32 assoc;
	/* Cache size in bytes. */
	u32 size;
};

/* CPU feature test macros.
   You only need to pass in the cu_res_cpu structure.
   If it is a pointer, simply dereference it in the parameter. */
#define CU_RES_C1(i,b) ((i).fx86_ecx1 & (CU_UPSHIFT(1u,b)))
#define CU_RES_D1(i,b) ((i).fx86_edx1 & (CU_UPSHIFT(1u,b)))
#define CU_RES_B7(i,b) ((i).fx86_ebx7 & (CU_UPSHIFT(1u,b)))
#define CU_RES_B7I(i,b) (CU_RES_B7(i,b) && (i).vendor_id == 1)
#define CU_RES_C7(i,b) ((i).fx86_ecx7 & (CU_UPSHIFT(1u,b)))
#define CU_RES_C8(i,b) ((i).fx86_ecx81 & (CU_UPSHIFT(1u,b)))
#define CU_RES_C8A(i,b) (CU_RES_C8(i,b) && (i).vendor_id == 2)
#define CU_RES_D8(i,b) ((i).fx86_edx81 & (CU_UPSHIFT(1u,b)))
#define CU_RES_D8A(i,b) (CU_RES_D8(i,b) && (i).vendor_id == 2)

/* See the Wikipedia page on the CPUID instruction for more information on each feature. */

#define CU_RES_SSE3(i) CU_RES_C1(i,0)
#define CU_RES_PCLMULQDQ(i) CU_RES_C1(i,1)
#define CU_RES_DTES64(i) CU_RES_C1(i,2)
#define CU_RES_MONITOR(i) CU_RES_C1(i,3)
#define CU_RES_DS_CPL(i) CU_RES_C1(i,4)
#define CU_RES_VMX(i) CU_RES_C1(i,5)
#define CU_RES_SMX(i) CU_RES_C1(i,6)
#define CU_RES_EST(i) CU_RES_C1(i,7)
#define CU_RES_TM2(i) CU_RES_C1(i,8)
#define CU_RES_SSSE3(i) CU_RES_C1(i,9)
#define CU_RES_CNXTID(i) CU_RES_C1(i,10)
#define CU_RES_SDBG(i) CU_RES_C1(i,11)
#define CU_RES_FMA(i) CU_RES_C1(i,12)
#define CU_RES_CX16(i) CU_RES_C1(i,13)
#define CU_RES_XTPR(i) CU_RES_C1(i,14)
#define CU_RES_PDCM(i) CU_RES_C1(i,15)
#define CU_RES_PCID(i) CU_RES_C1(i,17)
#define CU_RES_DCA(i) CU_RES_C1(i,18)
#define CU_RES_SSE4_1(i) CU_RES_C1(i,19)
#define CU_RES_SSE4_2(i) CU_RES_C1(i,20)
#define CU_RES_X2APIC(i) CU_RES_C1(i,21)
#define CU_RES_MOVBE(i) CU_RES_C1(i,22)
#define CU_RES_POPCNT(i) CU_RES_C1(i,23)
#define CU_RES_TSC_DEADLINE(i) CU_RES_C1(i,24)
#define CU_RES_AES_NI(i) CU_RES_C1(i,25)
#define CU_RES_XSAVE(i) CU_RES_C1(i,26)
#define CU_RES_OSXSAVE(i) CU_RES_C1(i,27)
#define CU_RES_AVX(i) CU_RES_C1(i,28)
#define CU_RES_F16C(i) CU_RES_C1(i,29)
#define CU_RES_RDRND(i) CU_RES_C1(i,30)
#define CU_RES_FPU(i) CU_RES_D1(i,0)
#define CU_RES_VME(i) CU_RES_D1(i,1)
#define CU_RES_DE(i) CU_RES_D1(i,2)
#define CU_RES_PSE(i) CU_RES_D1(i,3)
#define CU_RES_TSC(i) CU_RES_D1(i,4)
#define CU_RES_MSR(i) CU_RES_D1(i,5)
#define CU_RES_PAE(i) CU_RES_D1(i,6)
#define CU_RES_MCE(i) CU_RES_D1(i,7)
#define CU_RES_CX8(i) CU_RES_D1(i,8)
#define CU_RES_APIC(i) CU_RES_D1(i,9)
#define CU_RES_SEP(i) CU_RES_D1(i,11)
#define CU_RES_MTRR(i) CU_RES_D1(i,12)
#define CU_RES_PGE(i) CU_RES_D1(i,13)
#define CU_RES_MCA(i) CU_RES_D1(i,14)
#define CU_RES_CMOV(i) CU_RES_D1(i,15)
#define CU_RES_PAT(i) CU_RES_D1(i,16)
#define CU_RES_PSE_36(i) CU_RES_D1(i,17)
#define CU_RES_CLFSH(i) CU_RES_D1(i,19)
#define CU_RES_DS(i) CU_RES_D1(i,21)
#define CU_RES_ACPI(i) CU_RES_D1(i,22)
#define CU_RES_MMX(i) CU_RES_D1(i,23)
#define CU_RES_FXSR(i) CU_RES_D1(i,24)
#define CU_RES_SSE(i) CU_RES_D1(i,25)
#define CU_RES_SSE2(i) CU_RES_D1(i,26)
#define CU_RES_SS(i) CU_RES_D1(i,27)
#define CU_RES_HTT(i) CU_RES_D1(i,28)
#define CU_RES_TM(i) CU_RES_D1(i,29)
#define CU_RES_PBE(i) CU_RES_D1(i,31)
#define CU_RES_FSGSBASE(i) CU_RES_B7(i,0)
#define CU_RES_SGX(i) CU_RES_B7(i,2)
#define CU_RES_BMI1(i) CU_RES_B7(i,3)
#define CU_RES_HLE(i) CU_RES_B7I(i,4)
#define CU_RES_AVX2(i) CU_RES_B7(i,5)
#define CU_RES_SMEP(i) CU_RES_B7(i,7)
#define CU_RES_BMI2(i) CU_RES_B7(i,8)
#define CU_RES_ERMS(i) CU_RES_B7(i,9)
#define CU_RES_INVPCID(i) CU_RES_B7(i,10)
#define CU_RES_RTM(i) CU_RES_B7I(i,11)
#define CU_RES_MPX(i) CU_RES_B7(i,14)
#define CU_RES_AVX512_F(i) CU_RES_B7(i,16)
#define CU_RES_AVX512_DQ(i) CU_RES_B7(i,17)
#define CU_RES_RDSEED(i) CU_RES_B7(i,18)
#define CU_RES_ADX(i) CU_RES_B7(i,19)
#define CU_RES_AVX512_IFMA(i) CU_RES_B7(i,21)
#define CU_RES_CLFLUSHOPT(i) CU_RES_B7(i,23)
#define CU_RES_CLWB(i) CU_RES_B7(i,24)
#define CU_RES_AVX512_PF(i) CU_RES_B7(i,26)
#define CU_RES_AVX512_ER(i) CU_RES_B7(i,27)
#define CU_RES_AVX512_CD(i) CU_RES_B7(i,28)
#define CU_RES_SHA(i) CU_RES_B7(i,29)
#define CU_RES_AVX512_BW(i) CU_RES_B7(i,30)
#define CU_RES_AVX512_VL(i) CU_RES_B7(i,31)
#define CU_RES_PREFTCHWT1(i) CU_RES_C7(i,0)
#define CU_RES_AVX512_VBMI(i) CU_RES_C7(i,1)
#define CU_RES_PKU(i) CU_RES_C7(i,3)
#define CU_RES_OSPKE(i) CU_RES_C7(i,4)
#define CU_RES_WAITPKG(i) CU_RES_C7(i,5)
#define CU_RES_AVX512_VBMI2(i) CU_RES_C7(i,6)
#define CU_RES_SHSTK(i) CU_RES_C7(i,7)
#define CU_RES_GFNI(i) CU_RES_C7(i,8)
#define CU_RES_VAES(i) CU_RES_C7(i,9)
#define CU_RES_VPCLMULQDQ(i) CU_RES_C7(i,10)
#define CU_RES_AVX512_VNNI(i) CU_RES_C7(i,11)
#define CU_RES_AVX512_BITALG(i) CU_RES_C7(i,12)
#define CU_RES_AVX512_VPOPCNTDQ(i) CU_RES_C7(i,14)
#define CU_RES_RDPID(i) CU_RES_C7(i,22)
#define CU_RES_CLDEMOTE(i) CU_RES_C7(i,25)
#define CU_RES_MOVDIRI(i) CU_RES_C7(i,27)
#define CU_RES_MOVDIR64B(i) CU_RES_C7(i,28)
#define CU_RES_ENQCMD(i) CU_RES_C7(i,29)
#define CU_RES_LAHF_LM(i) CU_RES_C8(i,0)
#define CU_RES_ABM(i) CU_RES_C8A(i,5)
#define CU_RES_SSE4A(i) CU_RES_C8A(i,6)
#define CU_RES_3DNOWPREFETCH(i) CU_RES_C8(i,8)
#define CU_RES_XOP(i) CU_RES_C8A(i,11)
#define CU_RES_LWP(i) CU_RES_C8(i,15)
#define CU_RES_FMA4(i) CU_RES_C8(i,16)
#define CU_RES_TBM(i) CU_RES_C8A(i,21)
#define CU_RES_MONITORX(i) CU_RES_C8(i,29)
#define CU_RES_MMXEXT(i) CU_RES_D8(i,22)
#define CU_RES_LM(i) CU_RES_D8(i,29)
#define CU_RES_3DNOWEXT(i) CU_RES_D8A(i,30)
#define CU_RES_3DNOW(i) CU_RES_D8A(i,31)

/* CPU general information.
   Use the cu_res_cpuinfo function to retrieve specific information.
   If any information is unable to be retrieved, it will be left as 0. */
typedef struct cu_res_cpu
{
	char name[49];

	/* Start of x86-specific data. */

	/* Vendor string as determined from the CPUID instruction.
	   Common examples include "GenuineIntel" and "AuthenticAMD". */
	char vendor[15];
	/* CPU identification IDs. */
	u32 stepping_id, family_id, model_id;
	/* Maximum CPUID supported leaf values. */
	u32 cpuid_base_max, cpuid_ext_max;

	/* CPU features bitfield. You can use the provided macros above for simplicity. */
	u32 fx86_ecx1, fx86_edx1;
	u32 fx86_ebx7, fx86_ecx7;
	u32 fx86_ecx81, fx86_edx81;

	/* Simple ID value for x86 'vendor'. 0 = Unknown/other, 1 = GenuineIntel, 2 = AuthenticAMD. */
	u32 vendor_id;

	/* End of x86-specific data. */

	/* Clock speeds. */
	u64 base_freq_hz; /* The base frequency, measured in Hertz. */
	u64 cur_freq_hz; /* The current frequency, measured in Hertz. */
	u64 min_freq_hz; /* The minimum frequency, measured in Hertz. */
	u64 max_freq_hz; /* The maximum frequency, measured in Hertz. */

	/* Processor counts. */
	u32 phyiscal_cores; /* The number of physical cores. */
	u32 logical_cores; /* The number of logical cores (aka threads). This could be higher than the physical core count, such as from hyperthreading. */

	/* Cache information for each level/type.
	   If level 1 cache is not split up into instruction and data cache (e.g. unified),
	   or it could not determine which l1 cache to use, then l1i will be filled with the cache data.

	   Non-shared caches show the size of one instance rather than the accumulated size of that cache type/level.
	   For example, if you have 4 cores with an L1i cache of 32KB, then l1i will show the size to be 32KB rather than 32KB*4. */
	struct cu_res_cpu_cache l1i, l1d, l2, l3, l4;

	/* 0 = unknown, 1 = x86, 2 = ARM. */
	u32 arch;
} cu_res_cpu;

/* Maximum size (including terminator) of string needed by byte formatting. */
#define CU_RES_BYTEFMT_MAXSIZE 8

/* Sets string to formatted digital storage text of given bytes count.
   Maximum terminated size is given by the CU_RES_BYTEFMT_MAXSIZE macro.
   A terminator is added to the end. Returns the given string. */
A_NTL((1)) char *cu_res_bytefmt(char *str, u64 bytes);

/* Get cryptographically secure random bytes if available.
   Returns number of random bytes written. */
A_NTL((1)) uptr cu_res_crypto(void *data, uptr bytes);

/* Get memory usage information. */
A_NTL((1)) int cu_res_meminfo(cu_res_mem *info);

/* Get CPU usage percentage by this current process.
   Requires consistent calling to provide an accurate percentage. */
A_NTH A_WUR real64 cu_res_cpuusage(void);

/* Get general CPU information.
   You can selectively choose which information to retrieve (set feature int argument to non-zero).
   'idfeatures' = Identification values (name, x86 vendor, revision, etc) and features supported (use macros provided).
   'corecache' = Size, line and associativity of each cache level/type present, as well as processor core counts.
   'speed' = Clock speed information, all measured in Hz.
   All other members will be cleared via memset.
   If it is unable to retrieve some information, it will remain cleared.
   The 'arch' member will always be set appropriately after calling this. */
A_NTL((1)) void cu_res_cpuinfo(cu_res_cpu *info, int idfeatures, int corecache, int speed);

/* Maximum size (including terminator) of string needed for OS, computer or user name. */
#define CU_RES_NAME_MAXSIZE 257

/* Get OS name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
A_NTH uptr cu_res_osname(char *namebuf);
/* Get host name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
A_NTH uptr cu_res_hostname(char *namebuf);
/* Get current user's name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
A_NTH uptr cu_res_username(char *namebuf);

#endif

/* ========================= Time ======================== */

#if CU_SETTING_TIME_FUNCS

#include <time.h>

/* Current date & time data structure. */
typedef struct cu_ctime
{
	struct tm m; /* Main time structure. */

	const char *tznm; /* Abbreviated timezone name. */
	int utcdif;    /* Seconds after UTC. */

	int nanosec;   /* [0-1K) */
	int microsec;  /* [0-1K) */
	int millisec;  /* [0-1K) */
} cu_ctime;

typedef struct cu_timer
{
	u64 nsecs; /* Current nanosecond loop. [0-1B) */
	u64 secs; /* Total seconds passed. */
} cu_timer;

/* Get current time in a date format. */
A_NTL((1)) void cu_time_now(cu_ctime *tm);

/* Get given timestamp in a date format. */
A_NTL((1)) void cu_time_date(cu_ctime *tm, i64 timestamp);
/* Get current sub-second timestamps (ms, us and ns) in a date format. */
A_NTL((1)) void cu_time_subsec(cu_ctime *tm);

/* Fill the given timer with current time. */
A_NTL((1)) void cu_timer_fill(cu_timer *tm);

/* Get the number of nanoseconds passed between two timers. */
A_NNULL((1,2)) A_WUR u64 cu_timer_dif(const cu_timer *A_RES start, const cu_timer *A_RES end);

/* Convert cu_timer_dif's nanoseconds result to different units of time. */
#define CU_TIMEDIF_CONV(dif, unit) ((real64)(dif) / (real64)(unit))

/* Number of nanoseconds in a microsecond. */
#define CU_TIME_USEC 1000
/* Number of nanoseconds in a millisecond. */
#define CU_TIME_MSEC 1000000
/* Number of nanoseconds in a second. */
#define CU_TIME_SEC  1000000000

/* Integral amount of milliseconds stored by a cu_timer. */
#define CU_TIMER_MSECS(tm) ((tm.nsecs / CU_TIME_MSEC) + secs * 1000)
/* Integral amount of microseconds stored by a cu_timer. */
#define CU_TIMER_USECS(tm) ((tm.nsecs / CU_TIME_USEC) + secs * 1000000)

#endif

/* ========================= Networking ======================== */

#if CU_SETTING_NETWORK_FUNCS

#if !CU_OS_UNIX
   typedef uptr cu_socket;
#else
   typedef int cu_socket;
#endif

#define CU_NET_IPADDR_LEN 46
/* Create retry value with millisecond delay [0,65535] and retry count [0,65535]. */
#define CU_NET_RETRYVAL(num_retries, retry_delay_msec) CU_UPSHIFT(num_retries, 16) + retry_delay_msec

/* Identifiers for listen events. */
enum cu_net_event
{
	/* 'data' and 'n' of the remote event function pointer arguments are left as NULL and 0 respectively in the function pointer call unless stated otherwise. */

	/* Data has been recieved from the target remote.
	   'data' (malloc'd) and 'n' are filled with recieved data and number of bytes respectively. */
	CUEVT_MESSAGE,
	/* (TCP and server only) Client connected to server.
	   'n' is set to the updated number of clients. */
	CUEVT_CONNECT,
	/* (TCP only) Target remote disconnected.
	   On a client, the listening function returns after this event.
	   On a server, 'n' is set to the updated number of clients. */
	CUEVT_DISCONNECT,
	/* This event can be run at a fixed interval. */
	CUEVT_HEARTBEAT
};

/* Mode bitmask. */
enum
{
	/* These are for the argument 'mode' in both 'listen' functions. */

	/* Use UDP sockets instead of TCP. */
	CU_NETMODE_UDP = 1,
	/* Use IPv6 instead of IPv4. */
	CU_NETMODE_IPV6 = 2
};

/* Information on an open connection.
   In a TCP or UDP client setting, you can modify 'user' and it will persist for the next time there is an event involving the remote.
   You can also store a pointer to this structure for later use (e.g. sending messages and closing outside of events).
   In a UDP server setting, the remote from CUEVT_MESSAGE is temporary so only the IP can be used to identify the client.
   It is recommended to use the temporary remote to send messages. If you want to send a message outside of events,
   you should store a copy of the given remote. */
typedef struct cu_net_remote
{
	/* (TCP only) Pointer that persists for this remote. */
	void *user;
	void *queued, *internal;
	int nqueue;
	cu_socket fd;
	/* IP information. Use cu_net_ipinfo to get the IP address as a string. */
	u16 ip_info[64];
	uptr mode;
} cu_net_remote;

/* Event handler function type. On a client, 'remote' is always NULL. */
typedef void (*cu_event_handler)(cu_net_remote *A_RES server, cu_net_remote *A_RES remote, enum cu_net_event event_type, void *A_RES data, uptr n);

/* Initialize networking functions. */
A_NTH int cu_net_init(void);
/* Terminate networking functions. */
A_NTH void cu_net_terminate(void);

/* Connects to a server and listens for network events.
   You can retry a specific number times with a specific delay using CU_NET_RETRYVAL.
   The following events can occur: CUEVT_DISCONNECT (TCP only), CUEVT_HEARTBEAT (unless delay is negative) and CUEVT_MESSAGE.
   Returns 0 if failed to connect, -1 on other error. Otherwise, it blocks until the server disconnects (CUEVT_DISCONNECT) then returns 1. */
A_NTL((1,2,3,7)) int cu_client_listen(cu_net_remote *A_RES server, const char *A_RES address, const char *A_RES port, void *A_RES user_ptr, u32 retryval, uptr mode, cu_event_handler ehandler, int heartbeat_delay_msec);

/* Starts a server and listens for network events.
   The following events can occur: CUEVT_DISCONNECT (TCP only), CUEVT_CONNECT (TCP only), CUEVT_HEARTBEAT (unless delay is negative) and CUEVT_MESSAGE.
   Returns 0 if failed to start. Otherwise, it blocks until the server is closed then returns 1. */
A_NTL((1,2,6)) int cu_server_listen(cu_net_remote *A_RES server, const char *A_RES port, void *A_RES user_ptr, uptr mode, uptr tcp_maxclients, cu_event_handler ehandler, int heartbeat_delay_msec);

/* (TCP only) Sends a message to all connected clients, excluding those in the 'except' list. */
A_NTL((1,2)) void cu_server_broadcast(const cu_net_remote *A_RES server, const void *A_RES data, uptr bytes, cu_net_remote *A_RES *A_RES except, uptr except_len);

/* Queue a message to send to the given remote.
   If 'data' is allocated and not needed anymore, remember to free it.
   Returns 0 on error. */
A_NTL((1,2)) int cu_net_sendmsg(cu_net_remote *A_RES target, const void *A_RES data, uptr n);

/* Fills 'ipbuf' with the IP address of the given remote.
   At most CU_NET_IPADDR_LEN bytes are written, including the terminator.
   Returns NULL on error, otherwise 'ipbuf'. */
A_NTL((1,2)) char *cu_net_ipinfo(const cu_net_remote *A_RES remote, char *A_RES ipbuf);

/* Closes the given remote.
   The CUEVT_DISCONNECT event will run afterwards, unless running a server and closing the server itself.
   In this case, all clients will be disconnected without an event and the listening will stop. */
A_NTL((1)) void cu_net_close(cu_net_remote *remote);

/* Sets the given string to the device's local interfaces.
   At most CU_NET_IPADDR_LEN bytes of string is used, including the terminator.
   A specific interface format can be requested using the CU_NET_INTERFACE_* macros.
   You can increase 'id' to use another found interface, starting from id 0.
   Returns the given string on success or NULL on error. */
A_NTL((1)) char *cu_net_interfaces(char *ipbuf, int if_fmt, int id);

/* Returns a string describing the last encountered error. */
A_NTH A_WUR CU_ATTRIB_RETURNS_NONNULL const char *cu_net_lasterr(void);

#define CU_NET_INTERFACE_ANY 0
#define CU_NET_INTERFACE_IPV4 1
#define CU_NET_INTERFACE_IPV6 2

#endif

/* ========================= Threading ======================== */

#if CU_SETTING_THREAD_FUNCS

#if CU_THREAD_POSIX_AVAILABLE
#  define CU_THREAD_POSIX_USED 1
#  define CU_THREAD_WIN_USED 0
#  define CU_THREAD_C_USED 0
#  include <pthread.h>
   typedef pthread_t cu_thread;
   typedef pthread_mutex_t cu_thread_mutex;
   typedef pthread_cond_t cu_thread_cond;
   typedef void *cu_thread_return;
   typedef cu_thread_return (*cu_thread_func)(void *);
#  define CU_THREAD_FUNCTION(name, argname) cu_thread_return name(void *argname)
#elif CU_THREAD_WIN_AVAILABLE
#  define CU_THREAD_POSIX_USED 0
#  define CU_THREAD_WIN_USED 1
#  define CU_THREAD_C_USED 0
#  include <windows.h>
   typedef HANDLE cu_thread;
   typedef CRITICAL_SECTION cu_thread_mutex;
   typedef CONDITION_VARIABLE cu_thread_cond;
   typedef DWORD cu_thread_return;
   typedef cu_thread_return (WINAPI *cu_thread_func)(void *);
#  define CU_THREAD_FUNCTION(name, argname) cu_thread_return WINAPI name(void *argname)
#elif CU_THREAD_C_AVAILABLE
#  define CU_THREAD_POSIX_USED 0
#  define CU_THREAD_WIN_USED 0
#  define CU_THREAD_C_USED 1
#  include <threads.h>
   typedef thrd_t cu_thread;
   typedef mtx_t cu_thread_mutex;
   typedef cnd_t cu_thread_cond;
   typedef int cu_thread_return;
   typedef cu_thread_return (*cu_thread_func)(void *);
#  define CU_THREAD_FUNCTION(name, argname) cu_thread_return name(void *argname)
#endif

#define CU_THREAD_RETURN_VAL ((cu_thread_return)0)

/* Creates a thread. Returns 0 if failed. */
A_NTL((1)) cu_thread cu_thread_create(cu_thread_func function, void *arg);
/* Waits for the given thread to finish. Returns 0 if failed. */
A_NTH int cu_thread_join(cu_thread thread);
/* Detaches the given thread. Returns 0 if failed. */
A_NTH int cu_thread_detach(cu_thread thread);

/* Sleeps for the given number of nanoseconds.
   This does not affect timers. */
A_NTH void cu_thread_sleep(u64 nsecs, u64 secs);
/* Returns the number of CPUs available. */
A_NTH int cu_thread_count(void);

/* Initializes the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_mutex_init(cu_thread_mutex *mutex);
/* Locks the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_mutex_lock(cu_thread_mutex *mutex);
/* Unlocks the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_mutex_unlock(cu_thread_mutex *mutex);
/* Tries to lock the given mutex. Returns 0 if not acquired. */
A_NTL((1)) int cu_thread_mutex_trylock(cu_thread_mutex *mutex);
/* Destroys the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_mutex_destroy(cu_thread_mutex *mutex);

/* Initializes the given condition variable. Returns 0 if failed. */
A_NTL((1)) int cu_thread_cond_init(cu_thread_cond *cond);
/* Waits on the given condition variable. Returns 0 if failed. */
A_NTL((1)) int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex);
/* Unlocks the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_cond_signal(cu_thread_cond *cond);
/* Tries to lock the given mutex. Returns 0 if failed. */
A_NTL((1)) int cu_thread_cond_broadcast(cu_thread_cond *cond);
/* Destroys the given condition variable. Returns 0 if failed. */
A_NTL((1)) int cu_thread_cond_destroy(cu_thread_cond *cond);

typedef struct cu_thread_split_arg
{
	/* Inclusive starting index of work. */
	u64 start_index;
	/* Exclusive ending index of work. */
	u64 end_index;
	/* Thread-specific argument. */
	void *thread_arg;
} cu_thread_split_arg;

/* Splits indexed work evenly into multiple threads and blocks until all of them complete.
   The thread function's argument is a cu_thread_split_arg pointer, which will contain
   the thread-specific indices and the individual thread argument given through the double void pointer.
   Returns 0 if invalid arguments were given. */
A_NTL((1)) int cu_thread_split(cu_thread_func func, u64 work_count, void **each_thread_arg, int thread_count);

typedef struct cu_thread_pool
{
	/* The jobs to do. */
	void *jobs;
	/* Number of threads the pool was initialized with. */
	int nthreads;

	int jobsig;
	cu_thread_mutex mutex;
	cu_thread_cond cond;
	cu_thread *thrs;
} cu_thread_pool;

/* Initializes the given thread pool with the given number of threads.
   Returns 0 on initialization error or if an invalid thread count is given. */
A_NTL((1)) int cu_thread_pool_init(cu_thread_pool *pool, int nthreads);
/* Adds a job to the thread pool.
   A thread will execute the function with the provided argument. */
A_NTL((1,2)) int cu_thread_pool_add(cu_thread_pool *pool, cu_thread_func job, void *arg);
/* Waits for all threads to join and destroys the thread pool. */
A_NTL((1)) void cu_thread_pool_destroy(cu_thread_pool *pool);

/* Returns the current thread. */
A_NTH A_WUR cu_thread cu_thread_self(void);
/* Returns the current process ID. */
A_NTH A_WUR u64 cu_thread_pid(void);
/* Returns the current thread ID. */
A_NTH A_WUR u64 cu_thread_tid(void);

#endif /* CU_SETTING_THREAD_FUNCS */

#undef A_NNULL
#undef A_NTH
#undef A_WUR
#undef A_NTL
#undef A_RES

#endif /* CU_SETTING_FUNCS */

#ifdef __cplusplus
}
#endif

#endif /* CUTILS_HDR */
