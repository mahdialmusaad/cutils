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

#define CUTILS_VERSION 2

#if defined (__cplusplus)
extern "C" {
#endif

/* ==========================================================================
 *
 * ------------------------------ User settings -----------------------------
 *
 * ========================================================================== */

#if !defined (CU_SETTING_FUNCS)
#  define CU_SETTING_FUNCS 1
#endif

#if CU_SETTING_FUNCS
#  if !defined (CU_SETTING_FILE_FUNCS)
#    define CU_SETTING_FILE_FUNCS 1
#  endif
#  if !defined (CU_SETTING_RAND_FUNCS)
#    define CU_SETTING_RAND_FUNCS 1
#  endif
#  if !defined (CU_SETTING_RESOURCES_FUNCS)
#    define CU_SETTING_RESOURCES_FUNCS 1
#  endif
#  if !defined (CU_SETTING_TIME_FUNCS)
#    define CU_SETTING_TIME_FUNCS 1
#  endif
#  if !defined (CU_SETTING_NETWORK_FUNCS)
#    define CU_SETTING_NETWORK_FUNCS 1
#  endif
#  if !defined (CU_SETTING_THREAD_FUNCS)
#    define CU_SETTING_THREAD_FUNCS 1
#  endif
#  if !defined (CU_SETTING_ALLOC_FUNCS)
#    define CU_SETTING_ALLOC_FUNCS 1
#  endif
#else
#  define CU_SETTING_FILE_FUNCS 0
#  define CU_SETTING_RAND_FUNCS 0
#  define CU_SETTING_RESOURCES_FUNCS 0
#  define CU_SETTING_TIME_FUNCS 0
#  define CU_SETTING_NETWORK_FUNCS 0
#  define CU_SETTING_THREAD_FUNCS 0
#  define CU_SETTING_ALLOC_FUNCS 0
#endif

#if !defined (CU_SETTING_FORCE_DEBUG)
#  define CU_SETTING_FORCE_DEBUG 0
#endif

#if !defined (CU_SETTING_QUIET)
#  define CU_SETTING_QUIET 0
#endif

#if !defined (CU_SETTING_CUSTOM_ALLOCS)
#  define CU_SETTING_CUSTOM_ALLOCS CU_SETTING_ALLOC_FUNCS
#endif

/* ==========================================================================
 *
 * ------------------------- Compiler identification ------------------------
 *
 * ========================================================================== */

#define CU_COMPVER_MAKE(major, minor, patch) (((major) * 1000000) + ((minor) * 1000) + (patch))
#define CU_COMPVERP(compiler, major, minor, patch) (CU_COMP_ ## compiler >= CU_COMPVER_MAKE(major, minor, patch))
#define CU_COMPVER(compiler, major, minor) (CU_COMP_ ## compiler >= CU_COMPVER_MAKE(major, minor, 0))

#define CU_COMPILER_ISVER(compiler, major, minor) CU_COMPVER(compiler, major, minor)

#define CU_COMPILER_MAJOR(compiler) ((CU_COMP_ ## compiler) / 1000000)
#define CU_COMPILER_MINOR(compiler) (((CU_COMP_ ## compiler) / 1000) % 1000)
#define CU_COMPILER_PATCH(compiler) ((CU_COMP_ ## compiler) % 1000)

#if defined (_ACC_)
#  define CU_COMP_ACC 1
#else
#  define CU_COMP_ACC 0
#endif

#if defined (__CMB__)
#  define CU_COMP_ALTIUM_MBC CU_COMPVER_MAKE(__VERSION__ / 1000, __REVISION__, 0)
#else
#  define CU_COMP_ALTIUM_MBC 0
#endif

#if defined (__CHC__)
#  define CU_COMP_ALTIUM_CTH CU_COMPVER_MAKE(__VERSION__ / 1000, __REVISION__, 0)
#else
#  define CU_COMP_ALTIUM_CTH 0
#endif

#if defined (__ACK__)
#  define CU_COMP_AMSTERDAM 1
#else
#  define CU_COMP_AMSTERDAM 0
#endif

#if defined (__CC_ARM) && defined (__ARMCOMPILER_VERSION)
#  define CU_COMP_ARM CU_COMPVER_MAKE(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined (__CC_ARM) && defined (__ARMCC_VERSION)
#  define CU_COMP_ARM CU_COMPVER_MAKE(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#else
#  define CU_COMP_ARM 0
#endif

#if defined (AZTEC_C) || defined (__AZTEC_C__)
#  define CU_COMP_AZTEC CU_COMPVER_MAKE(__VERSION / 100, __VERSION % 100, 0)
#else
#  define CU_COMP_AZTEC 0
#endif

#if defined (__BORLANDC__) || defined (__CODEGEARC__)
#  define CU_COMP_BORLAND CU_COMPVER_MAKE(__BORLANDC__ / 0x100, __BORLANDC__ % 0x100, 0)
#else
#  define CU_COMP_BORLAND 0
#endif

#if defined (__CC65__)
#  define CU_COMP_CC65 CU_COMPVER_MAKE(__CC65__ / 0x100, __CC65__ % 0x100, 0)
#else
#  define CU_COMP_CC65 0
#endif

#if defined (__chibicc__)
#  define CU_COMP_CHIBICC 1
#else
#  define CU_COMP_CHIBICC 0
#endif

#if defined (__clang__)
#  define CU_COMP_CLANG CU_COMPVER_MAKE(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#  define CU_COMP_CLANG 0
#endif

#if defined (__COMO__)
#  define CU_COMP_COMEAU __COMO_VERSION__
#else
#  define CU_COMP_COMEAU 0
#endif

#if defined (__DECC) && defined (__DECC_VER)
#  define CU_COMP_COMPAQ CU_COMPVER_MAKE(__DECC_VER / 10000000, (__DECC_VER / 100000) % 100, __DECC_VER % 10000)
#elif defined (__DECC)
#  define CU_COMP_COMPAQ 1
#else
#  define CU_COMP_COMPAQ 0
#endif

#if defined (__convexc__)
#  define CU_COMP_CONVEX 1
#else
#  define CU_COMP_CONVEX 0
#endif

#if defined (__COMPCERT__)
#  define CU_COMP_COMPCERT 1
#else
#  define CU_COMP_COMPCERT 0
#endif

#if defined (__COVERITY__)
#  define CU_COMP_COVERITY 1
#else
#  define CU_COMP_COVERITY 0
#endif

#if defined (_CRAYC) && defined (_RELEASE_PATCHLEVEL)
#  define CU_COMP_CRAY CU_COMPVER_MAKE(_RELEASE_MAJOR, _RELEASE_MINOR, _RELEASE_PATCHLEVEL)
#elif defined (_CRAYC)
#  define CU_COMP_CRAY CU_COMPVER_MAKE(_RELEASE_MAJOR, _RELEASE_MINOR, 0)
#else
#  define CU_COMP_CRAY 0
#endif

#if defined (__DCC__)
#  define CU_COMP_DIAB CU_COMPVER_MAKE(__VERSION_NUMBER__ / 1000, (__VERSION_NUMBER__ / 100) % 10, __VERSION_NUMBER__ % 100)
#else
#  define CU_COMP_DIAB 0
#endif

#if defined (_DICE)
#  define CU_COMP_DICE 1
#else
#  define CU_COMP_DICE 0
#endif

#if defined (__DMC__)
#  define CU_COMP_DMC CU_COMPVER_MAKE(__DMC__ >> 8, (__DMC__ >> 4) & 0xf, __DMC__ & 0xf)
#else
#  define CU_COMP_DMC 0
#endif

#if defined (__SYSC__)
#  define CU_COMP_DIGNUS CU_COMPVER_MAKE(__SYSC_VER__ / 10000, (__SYSC_VER__ / 100) % 100, __SYSC_VER__ % 100)
#else
#  define CU_COMP_DIGNUS 0
#endif

#if defined (__DJGPP__) || defined (__GO32__)
#  define CU_COMP_DJGPP CU_COMPVER_MAKE(__DJGPP__, __DJGPP_MINOR__, 0)
#else
#  define CU_COMP_DJGPP 0
#endif

#if defined (__EDG__)
#  define CU_COMP_EDG CU_COMPVER_MAKE(__EDG_VERSION__ / 100, __EDG_VERSION__ % 100, 0)
#else
#  define CU_COMP_EDG 0
#endif

#if defined (__PATHCC__)
#  define CU_COMP_EKOPATH CU_COMPVER_MAKE(__PATHCC__, __PATHCC_MINOR__, __PATHCC_PATCHLEVEL__)
#else
#  define CU_COMP_EKOPATH 0
#endif

#if defined (__EMSCRIPTEN__)
#  define CU_COMP_EMSCRIPTEN CU_COMPVER_MAKE(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#else
#  define CU_COMP_EMSCRIPTEN 0
#endif

#if defined (__FCC_VERSION)
#  define CU_COMP_FUJITSU 1
#else
#  define CU_COMP_FUJITSU 0
#endif

#if defined (__GNUC__) && defined (__GNUC_PATCHLEVEL__)
#  define CU_COMP_GNU CU_COMPVER_MAKE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined (__GNUC__)
#  define CU_COMP_GNU CU_COMPVER_MAKE(__GNUC__, __GNUC_MINOR__, 0)
#else
#  define CU_COMP_GNU 0
#endif

#if (defined (__ghs) || defined (__ghs__)) && defined (__GHS_VERSION_NUMBER__)
#  define CU_COMP_GHS __GHS_VERSION_NUMBER__
#elif defined (__ghs) || defined (__ghs__)
#  define CU_COMP_GHS __ghs
#else
#  define CU_COMP_GHS 0
#endif

#if defined (__HP_cc)
#  define CU_COMP_HPANSI 1
#else
#  define CU_COMP_HPANSI 0
#endif

#if defined (__HP_aCC)
#  define CU_COMP_HPACC CU_COMPVER_MAKE(__HP_aCC / 10000, (__HP_aCC / 100) % 100, __HP_aCC % 100)
#else
#  define CU_COMP_HPACC 0
#endif

#if defined (__IAR_SYSTEMS_ICC__)
#  if __VER__ > 1000
#    define CU_COMP_IAR CU_COMPVER_MAKE((__VER__ / 1000000), ((__VER__ / 1000) % 1000), (__VER__ % 1000))
#  else
#    define CU_COMP_IAR CU_COMPVER_MAKE(__VER__ / 100, __VER__ % 100, 0)
#  endif
#else
#  define CU_COMP_IAR 0
#endif

#if defined (__ibmxl__)
#  define CU_COMP_IBM CU_COMPVER_MAKE(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined (__xlC__) && defined (__xlC_ver__)
#  define CU_COMP_IBM CU_COMPVER_MAKE(__xlC__ >> 8, __xlC__ & 0xFF, (__xlC_ver__ >> 8) & 0xFF)
#elif defined (__xlC__)
#  define CU_COMP_IBM CU_COMPVER_MAKE(__xlC__ >> 8, __xlC__ & 0xFF, 0)
#else
#  define CU_COMP_IBM 0
#endif

#if defined (__IMAGECRAFT__)
#  define CU_COMP_IMAGECRAFT 1
#else
#  define CU_COMP_IMAGECRAFT 0
#endif

#if defined (__INTEL_COMPILER) && defined (__INTEL_COMPILER_UPDATE) && !defined (__ICL)
#  define CU_COMP_INTEL CU_COMPVER_MAKE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#elif defined (__INTEL_COMPILER) && !defined (__ICL)
#  define CU_COMP_INTEL CU_COMPVER_MAKE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#else
#  define CU_COMP_INTEL 0
#endif

#if defined (__KCC)
#  define CU_COMP_KAI CU_COMPVER_MAKE(__KCC_VERSION / 0x1000, (__KCC_VERSION / 0x100) % 0x10, __KCC_VERSION % 0x100)
#else
#  define CU_COMP_KAI 0
#endif

#if defined (__CA__) || defined (__KEIL__)
#  define CU_COMP_KEILCARM CU_COMPVER_MAKE(__CA__ / 100, __CA__ % 100, 0)
#else
#  define CU_COMP_KEILCARM 0
#endif

#if defined (__C166__)
#  define CU_COMP_KEILC166 CU_COMPVER_MAKE(__C166__ / 100, __C166__ % 100, 0)
#else
#  define CU_COMP_KEILC166 0
#endif

#if defined (__C51__)
#  define CU_COMP_KEILC51 CU_COMPVER_MAKE(__C51__ / 100, __C51__ % 100, 0)
#else
#  define CU_COMP_KEILC51 0
#endif

#if defined (__LCC__)
#  define CU_COMP_LCC 1
#else
#  define CU_COMP_LCC 0
#endif

#if defined (__llvm__)
#  define CU_COMP_LLVM 1
#else
#  define CU_COMP_LLVM 0
#endif

#if defined (__LCC__) && defined (__LCC_MINOR__)
#  define CU_COMP_MCST CU_COMPVER_MAKE(__LCC__ / 100, __LCC__ % 100, __LCC_MINOR__)
#else
#  define CU_COMP_MCST 0
#endif

#if defined (__HIGHC__)
#  define CU_COMP_METAWARE 1
#else
#  define CU_COMP_METAWARE 0
#endif

#if defined (__MWERKS__) && defined (__CWCC__)
#  define CU_COMP_METROWERKS CU_COMPVER_MAKE(__CWCC__ / 0x1000, (__CWCC__ / 0x100) % 0x10, __CWCC__ % 0x100)
#elif defined (__MWERKS__)
#  define CU_COMP_METROWERKS CU_COMPVER_MAKE(__MWERKS__ / 0x1000, (__MWERKS__ / 0x100) % 0x10, __MWERKS__ % 0x100)
#else
#  define CU_COMP_METROWERKS 0
#endif

#if defined (_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined (__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 1000)
#elif defined (_MSC_FULL_VER) && !defined (__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 10)
#elif defined (_MSC_VER) && !defined (__ICL)
#  define CU_COMP_MSVC CU_COMPVER_MAKE(_MSC_VER / 100, _MSC_VER % 100, 0)
#else
#  define CU_COMP_MSVC 0
#endif

#if defined (_MRI)
#  define CU_COMP_MICROTEC 1
#else
#  define CU_COMP_MICROTEC 0
#endif

#if defined (__NDPC__) || defined (__NDPX__)
#  define CU_COMP_MICROWAY 1
#else
#  define CU_COMP_MICROWAY 0
#endif

#if defined (__MINGW32__) || defined (__MINGW64__)
#  define CU_COMP_MINGW 1
#  if defined (__MINGW64__)
#    define CU_COMP_MINGW64 CU_COMPVER_MAKE(__MINGW64_MAJOR_VERSION, __MINGW64_MINOR_VERSION, 0)
#    define CU_COMP_MINGW32 0
#  else
#    define CU_COMP_MINGW64 1
#    define CU_COMP_MINGW32 CU_COMPVER_MAKE(__MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION, 0)
#  endif
#else
#  define CU_COMP_MINGW 0
#  define CU_COMP_MINGW64 0
#  define CU_COMP_MINGW32 0
#endif

#if (defined (__sgi) || defined (sgi)) && defined (_SGI_COMPILER_VERSION)
#  define CU_COMP_MIPSPRO CU_COMPVER_MAKE(_SGI_COMPILER_VERSION / 100, (_SGI_COMPILER_VERSION / 10) % 10, _SGI_COMPILER_VERSION % 10)
#elif (defined (__sgi) || defined (sgi)) && defined (_COMPILER_VERSION)
#  define CU_COMP_MIPSPRO CU_COMPVER_MAKE(_COMPILER_VERSION / 100, (_COMPILER_VERSION / 10) % 10, _COMPILER_VERSION % 10)
#elif defined (__sgi) || defined (sgi)
#  define CU_COMP_MIPSPRO 1
#else
#  define CU_COMP_MIPSPRO 0
#endif

#if defined (MIRACLE)
#  define CU_COMP_MIRACLE 1
#else
#  define CU_COMP_MIRACLE 0
#endif

#if defined (__MRC__) || defined (MPW_C)
#  define CU_COMP_MPW CU_COMPVER_MAKE(__MRC__ / 0x100, __MRC__ % 0x100, 0)
#else
#  define CU_COMP_MPW 0
#endif

#if defined (__MRISC32__)
#  define CU_COMP_MRISC 1
#else
#  define CU_COMP_MRISC 0
#endif

#if defined (__CC_NORCROFT)
#  define CU_COMP_NORCROFT __ARMCC_VERSION
#else
#  define CU_COMP_NORCROFT 0
#endif

#if defined (__NVCC__)
#  define CU_COMP_NVCC CU_COMPVER_MAKE(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
#else
#  define CU_COMP_NVCC 0
#endif

#if defined (__NWCC__)
#  define CU_COMP_NWCC 1
#else
#  define CU_COMP_NWCC 0
#endif

#if defined (__NVCOMPILER)
#  define CU_COMP_NVHPC 1
#else
#  define CU_COMP_NVHPC 0
#endif

#if defined (__OPEN64__) || defined (__OPENCC__)
#  define CU_COMP_OPEN64 CU_COMPVER_MAKE(__OPENCC__, __OPENCC_MINOR__, 0)
#else
#  define CU_COMP_OPEN64 0
#endif

#if defined (ORA_PROC)
#  define CU_COMP_ORACLE_PRECOMP 1
#else
#  define CU_COMP_ORACLE_PRECOMP 0
#endif

#if defined (__ORANGEC__)
#  define CU_COMP_ORANGE CU_COMPVER_MAKE(__ORANGEC_MAJOR__, __ORANGEC_MINOR__, __ORANGEC_PATCHLEVEL__)
#else
#  define CU_COMP_ORANGE 0
#endif

#if defined (__PACIFIC__)
#  define CU_COMP_PACIFIC 1
#else
#  define CU_COMP_PACIFIC 0
#endif

#if defined (_PACC_VER)
#  define CU_COMP_PALM CU_COMPVER_MAKE(_PACC_VER / 0x100000, (_PACC_VER / 0x1000) % 0x100, _PACC_VER % 0x100)
#else
#  define CU_COMP_PALM 0
#endif

#if defined (__POCC__)
#  define CU_COMP_PELLES CU_COMPVER_MAKE(__POCC__ / 100, __POCC__ % 100, 0)
#else
#  define CU_COMP_PELLES 0
#endif

#if defined (__PGI)
#  define CU_COMP_PGI CU_COMPVER_MAKE(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#else
#  define CU_COMP_PGI 0
#endif

#if (defined (__RENESAS__) || defined (__HITACHI__)) && defined (__HITACHI_VERSION__)
#  define CU_COMP_RENESAS CU_COMPVER_MAKE(__HITACHI_VERSION__ / 0x100, __HITACHI_VERSION__ % 0x100, 0)
#elif defined (__RENESAS__) || defined (__HITACHI__)
#  define CU_COMP_RENESAS CU_COMPVER_MAKE(__RENESAS_VERSION__ / 0x1000000, (__RENESAS_VERSION__ / 0x10000) % 0x100, __RENESAS_VERSION__ / 0x100)
#else
#  define CU_COMP_RENESAS 0
#endif

#if (defined (SASC) || defined (__SASC)) && defined (__SASC__)
#  define CU_COMP_SASC CU_COMPVER_MAKE(__SASC__ / 100, __SASC__ % 100, 0)
#elif defined (SASC) || defined (__SASC)
#  define CU_COMP_SASC CU_COMPVER_MAKE(__VERSION__, __REVISION__, 0)
#else
#  define CU_COMP_SASC 0
#endif

#if defined (_SCO_DS)
#  define CU_COMP_SCO 1
#else
#  define CU_COMP_SCO 0
#endif

#if defined (SDCC) || defined (__SDCC)
#  define CU_COMP_SDCC CU_COMPVER_MAKE(__SDCC_VERSION_MAJOR, __SDCC_VERSION_MINOR, __SDCC_VERSION_PATCH)
#else
#  define CU_COMP_SDCC 0
#endif

#if defined (__SNC__)
#  define CU_COMP_SNC 1
#else
#  define CU_COMP_SNC 0
#endif

#if defined (__SUNPRO_C) && (__SUNPRO_C > 0x1000)
#  define CU_COMP_SUNPRO CU_COMPVER_MAKE((((__SUNPRO_C >> 16) & 0xf) * 10) + ((__SUNPRO_C >> 12) & 0xf), (((__SUNPRO_C >> 8) & 0xf) * 10) + ((__SUNPRO_C >> 4) & 0xf), (__SUNPRO_C & 0xf) * 10)
#elif defined (__SUNPRO_C)
#  define CU_COMP_SUNPRO CU_COMPVER_MAKE((__SUNPRO_C >> 8) & 0xf, (__SUNPRO_C >> 4) & 0xf, (__SUNPRO_C) & 0xf)
#elif defined (__SUNPRO_CC) && (__SUNPRO_CC > 0x1000)
#  define CU_COMP_SUNPRO CU_COMPVER_MAKE((((__SUNPRO_CC >> 16) & 0xf) * 10) + ((__SUNPRO_CC >> 12) & 0xf), (((__SUNPRO_CC >> 8) & 0xf) * 10) + ((__SUNPRO_CC >> 4) & 0xf), (__SUNPRO_CC & 0xf) * 10)
#elif defined (__SUNPRO_CC)
#  define CU_COMP_SUNPRO CU_COMPVER_MAKE((__SUNPRO_CC >> 8) & 0xf, (__SUNPRO_CC >> 4) & 0xf, (__SUNPRO_CC) & 0xf)
#else
#  define CU_COMP_SUNPRO 0
#endif

#if defined (__SC__)
#  define CU_COMP_SYMTEC CU_COMPVER_MAKE(__SC__ / 0x100, __SC__ % 0x100, 0)
#else
#  define CU_COMP_SYMTEC 0
#endif

#if defined (__TenDRA__)
#  define CU_COMP_TENDRA 1
#else
#  define CU_COMP_TENDRA 0
#endif

#if defined (__TI_COMPILER_VERSION__) && (defined (__TMS470__) || defined (__TI_ARM__) || defined (__MSP430__) || defined (__TMS320C2000__))
#  if (__TI_COMPILER_VERSION__ >= 16000000)
#    define CU_COMP_TI CU_COMPVER_MAKE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#  else
#    define CU_COMP_TI 1
#  endif
#else
#  define CU_COMP_TI 0
#endif

#if defined (THINKC3) || defined (THINKC4)
#  if defined (THINKC4)
#    define CU_COMP_THINKC 4
#  else
#    define CU_COMP_THINKC 3
#  endif
#else
#  define CU_COMP_THINKC 0
#endif

#if defined (__TINYC__)
#  define CU_COMP_TINYC CU_COMPVER_MAKE(__TINYC__ / 1000, (__TINYC__ / 100) % 10, __TINYC__ % 100)
#else
#  define CU_COMP_TINYC 0
#endif

#if defined (__TURBOC__)
#  define CU_COMP_TURBO CU_COMPVER_MAKE(__TURBOC__ / 0x100, __TURBOC__ % 0x100, 0)
#else
#  define CU_COMP_TURBO 0
#endif

#if defined (_UCC)
#  define CU_COMP_UCC CU_COMPVER_MAKE(_MAJOR_REV, _MINOR_REV, 0)
#else
#  define CU_COMP_UCC 0
#endif

#if defined (__USLC__)
#  define CU_COMP_USLC CU_COMPVER_MAKE(__SCO_VERSION__ / 100000000, (__SCO_VERSION__ / 0x1000000) % 100, 0)
#else
#  define CU_COMP_USLC 0
#endif

#if defined (__VBCC__)
#  define CU_COMP_VBCC 1
#else
#  define CU_COMP_VBCC 0
#endif

#if defined (__VOSC__)
#  define CU_COMP_VOS (__VOSC__ + 1)
#else
#  define CU_COMP_VOS 0
#endif

#if defined (__WATCOMC__)
#  define CU_COMP_WATCOM CU_COMPVER_MAKE(__WATCOMC__ / 100, __WATCOMC__ % 100, 0)
#else
#  define CU_COMP_WATCOM 0
#endif

#if defined (__ZTC__)
#  define CU_COMP_ZORTECH 1
#else
#  define CU_COMP_ZORTECH 0
#endif

#if defined (__SCCZ80) || defined (SCCZ80)
#  define CU_COMP_Z88DK 1
#else
#  define CU_COMP_Z88DK 0
#endif

#define CU_COMP_GCC CU_COMP_GNU

/* ==========================================================================
 *
 * ---------------------------- OS identification ---------------------------
 *
 * ========================================================================== */

#if defined (_AIX)
#  if defined (__AIX43)
#    define CU_OS_AIX 43
#  elif defined (__AIX41)
#    define CU_OS_AIX 41
#  else
#    define CU_OS_AIX 32
#  endif
#  define CU_OS_UNIX_FORCE
#else
#  define CU_OS_AIX 0
#endif

#if defined (aegis)
#  define CU_OS_APOLLO_AEGIS 1
#else
#  define CU_OS_APOLLO_AEGIS 0
#endif

#if defined (AMIGA) || defined (__amigaos__)
#  if CU_COMP_GNU
#    define CU_OS_UNIX_FORCE
#  endif
#  define CU_OS_AMIGA 1
#else
#  define CU_OS_AMIGA 0
#endif

#if defined (apollo)
#  define CU_OS_APOLLO_DOMAIN 1
#else
#  define CU_OS_APOLLO_DOMAIN 0
#endif

#if defined (__APPLE__)
#  define CU_OS_APPLE 1
#else
#  define CU_OS_APPLE 0
#endif

#if defined (__BEOS__)
#  define CU_OS_BE 1
#else
#  define CU_OS_BE 0
#endif

#if defined (__bg__) || defined (__bgq__) || defined (__THW_BLUEGENE__)
#  if defined (__bgq__)
#    define CU_OS_BLUEGENE __bgq__
#  elif defined (__TOS_BGQ__)
#    define CU_OS_BLUEGENE __TOS_BGQ__
#  else
#    define CU_OS_BLUEGENE 1
#  endif
#else
#  define CU_OS_BLUEGENE 0
#endif

#if defined (BSD) || defined (_SYSTYPE_BSD)
#  if defined (BSD4_4)
#    define CU_OS_BSD 44
#  elif defined (BSD4_3)
#    define CU_OS_BSD 43
#  elif defined (BSD4_2)
#    define CU_OS_BSD 42
#  else
#    define CU_OS_BSD BSD
#  endif
#else
#  define CU_OS_BSD 0
#endif

#if defined (__bsdi__)
#  define CU_OS_BSD_BSDI 1
#else
#  define CU_OS_BSD_BSDI 0
#endif

#if defined (__DragonFly__)
#  define CU_OS_BSD_DRAGONFLY 1
#else
#  define CU_OS_BSD_DRAGONFLY 0
#endif

#if defined (__FreeBSD__)
#  define CU_OS_BSD_FREE __FreeBSD_version
#else
#  define CU_OS_BSD_FREE 0
#endif

#if defined (__NetBSD__) || defined (__NETBSD__)
#  if defined (NetBSD0_8)
#    define CU_OS_BSD_NET 8
#  elif defined (NetBSD0_9)
#    define CU_OS_BSD_NET 9
#  elif defined (NetBSD1_0)
#    define CU_OS_BSD_NET 10
#  else
#    define CU_OS_BSD_NET __NetBSD_Version
#  endif
#  define CU_OS_UNIX_FORCE
#else
#  define CU_OS_BSD_NET 0
#endif

#if defined (__OpenBSD__)
#  if defined (OpenBSD1_2)
#    define CU_OS_BSD_OPEN 12
#  elif defined (OpenBSD1_3)
#    define CU_OS_BSD_OPEN 13
#  elif defined (OpenBSD1_4)
#    define CU_OS_BSD_OPEN 14
#  elif defined (OpenBSD1_5)
#    define CU_OS_BSD_OPEN 15
#  elif defined (OpenBSD1_6)
#    define CU_OS_BSD_OPEN 16
#  elif defined (OpenBSD1_7)
#    define CU_OS_BSD_OPEN 17
#  elif defined (OpenBSD1_8)
#    define CU_OS_BSD_OPEN 18
#  elif defined (OpenBSD1_9)
#    define CU_OS_BSD_OPEN 19
#  elif defined (OpenBSD2_0)
#    define CU_OS_BSD_OPEN 20
#  elif defined (OpenBSD2_1)
#    define CU_OS_BSD_OPEN 21
#  elif defined (OpenBSD2_2)
#    define CU_OS_BSD_OPEN 22
#  elif defined (OpenBSD2_3)
#    define CU_OS_BSD_OPEN 23
#  elif defined (OpenBSD2_4)
#    define CU_OS_BSD_OPEN 24
#  elif defined (OpenBSD2_5)
#    define CU_OS_BSD_OPEN 25
#  elif defined (OpenBSD2_6)
#    define CU_OS_BSD_OPEN 26
#  elif defined (OpenBSD2_7)
#    define CU_OS_BSD_OPEN 27
#  elif defined (OpenBSD2_8)
#    define CU_OS_BSD_OPEN 28
#  elif defined (OpenBSD2_9)
#    define CU_OS_BSD_OPEN 29
#  elif defined (OpenBSD3_0)
#    define CU_OS_BSD_OPEN 30
#  elif defined (OpenBSD3_1)
#    define CU_OS_BSD_OPEN 31
#  elif defined (OpenBSD3_3)
#    define CU_OS_BSD_OPEN 33
#  elif defined (OpenBSD3_3)
#    define CU_OS_BSD_OPEN 33
#  elif defined (OpenBSD3_4)
#    define CU_OS_BSD_OPEN 34
#  elif defined (OpenBSD3_5)
#    define CU_OS_BSD_OPEN 35
#  elif defined (OpenBSD3_6)
#    define CU_OS_BSD_OPEN 36
#  elif defined (OpenBSD3_7)
#    define CU_OS_BSD_OPEN 37
#  elif defined (OpenBSD3_8)
#    define CU_OS_BSD_OPEN 38
#  elif defined (OpenBSD3_9)
#    define CU_OS_BSD_OPEN 39
#  elif defined (OpenBSD4_0)
#    define CU_OS_BSD_OPEN 40
#  endif
#endif
#if defined (__OpenBSD__) && !defined (CU_OS_BSD_OPEN)
#  if defined (OpenBSD4_1)
#    define CU_OS_BSD_OPEN 41
#  elif defined (OpenBSD4_4)
#    define CU_OS_BSD_OPEN 44
#  elif defined (OpenBSD4_3)
#    define CU_OS_BSD_OPEN 43
#  elif defined (OpenBSD4_4)
#    define CU_OS_BSD_OPEN 44
#  elif defined (OpenBSD4_5)
#    define CU_OS_BSD_OPEN 45
#  elif defined (OpenBSD4_6)
#    define CU_OS_BSD_OPEN 46
#  elif defined (OpenBSD4_7)
#    define CU_OS_BSD_OPEN 47
#  elif defined (OpenBSD4_8)
#    define CU_OS_BSD_OPEN 48
#  elif defined (OpenBSD4_9)
#    define CU_OS_BSD_OPEN 49
#  elif defined (OpenBSD5_0)
#    define CU_OS_BSD_OPEN 50
#  elif defined (OpenBSD5_1)
#    define CU_OS_BSD_OPEN 51
#  elif defined (OpenBSD5_5)
#    define CU_OS_BSD_OPEN 55
#  elif defined (OpenBSD5_3)
#    define CU_OS_BSD_OPEN 53
#  elif defined (OpenBSD5_4)
#    define CU_OS_BSD_OPEN 54
#  elif defined (OpenBSD5_5)
#    define CU_OS_BSD_OPEN 55
#  elif defined (OpenBSD5_6)
#    define CU_OS_BSD_OPEN 56
#  elif defined (OpenBSD5_7)
#    define CU_OS_BSD_OPEN 57
#  elif defined (OpenBSD5_8)
#    define CU_OS_BSD_OPEN 58
#  elif defined (OpenBSD5_9)
#    define CU_OS_BSD_OPEN 59
#  elif defined (OpenBSD6_0)
#    define CU_OS_BSD_OPEN 60
#  elif defined (OpenBSD6_1)
#    define CU_OS_BSD_OPEN 61
#  elif defined (OpenBSD6_6)
#    define CU_OS_BSD_OPEN 66
#  elif defined (OpenBSD6_3)
#    define CU_OS_BSD_OPEN 63
#  elif defined (OpenBSD6_4)
#    define CU_OS_BSD_OPEN 64
#  elif defined (OpenBSD6_5)
#    define CU_OS_BSD_OPEN 65
#  elif defined (OpenBSD6_6)
#    define CU_OS_BSD_OPEN 66
#  elif defined (OpenBSD6_7)
#    define CU_OS_BSD_OPEN 67
#  elif defined (OpenBSD6_8)
#    define CU_OS_BSD_OPEN 68
#  elif defined (OpenBSD6_9)
#    define CU_OS_BSD_OPEN 69
#  elif defined (OpenBSD7_0)
#    define CU_OS_BSD_OPEN 70
#  elif defined (OpenBSD7_1)
#    define CU_OS_BSD_OPEN 71
#  elif defined (OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined (OpenBSD7_3)
#    define CU_OS_BSD_OPEN 73
#  elif defined (OpenBSD7_4)
#    define CU_OS_BSD_OPEN 74
#  elif defined (OpenBSD7_5)
#    define CU_OS_BSD_OPEN 75
#  elif defined (OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined (OpenBSD7_7)
#    define CU_OS_BSD_OPEN 77
#  elif defined (OpenBSD7_8)
#    define CU_OS_BSD_OPEN 78
#  elif defined (OpenBSD7_9)
#    define CU_OS_BSD_OPEN 79
#  else
#    define CU_OS_BSD_OPEN 1
#  endif
#else
#  define CU_OS_BSD_OPEN 0
#endif

#if defined (__convex__)
#  define CU_OS_CONVEX 1
#else
#  define CU_OS_CONVEX 0
#endif

#if defined (__CYGWIN__)
#  define CU_OS_CYGWIN 1
#  define CU_OS_UNIX_FORCE
#else
#  define CU_OS_CYGWIN 0
#endif

#if defined (__DGUX) || defined (__DGUX__) || defined (__dgux__)
#  define CU_OS_DGUX 1
#else
#  define CU_OS_DGUX 0
#endif

#if defined (_SEQUENT_) || defined (sequent)
#  define CU_OS_DYNIX_PTX 1
#else
#  define CU_OS_DYNIX_PTX 0
#endif

#if defined (__ECOS)
#  define CU_OS_ECOS 1
#else
#  define CU_OS_ECOS 0
#endif

#if defined (__EMX__)
#  define CU_OS_EMX 1
#else
#  define CU_OS_EMX 0
#endif

#if defined (__GNU__) || defined (__gnu_hurd__)
#  define CU_OS_GNU 1
#else
#  define CU_OS_GNU 0
#endif

#if defined (__HAIKU__)
#  define CU_OS_HAIKU 1
#else
#  define CU_OS_HAIKU 0
#endif

#if defined (__hiuxmpp)
#  define CU_OS_HIUXMPP 1
#else
#  define CU_OS_HIUXMPP 0
#endif

#if defined (_hpux) || defined (hpux) || defined (__hpux)
#  define CU_OS_HPUX 1
#else
#  define CU_OS_HPUX 0
#endif

#if defined (__OS400__) || defined (__OS400_TGTVRM__)
#  define CU_OS_IBM400 __OS400_TGTVRM__
#else
#  define CU_OS_IBM400 0
#endif

#if defined (__INTEGRITY)
#  define CU_OS_INTEGRITY 1
#else
#  define CU_OS_INTEGRITY 0
#endif

#if defined (__INTERIX)
#  define CU_OS_INTERIX 1
#else
#  define CU_OS_INTERIX 0
#endif

#if defined (sgi) || defined (__sgi)
#  define CU_OS_IRIX 1
#else
#  define CU_OS_IRIX 0
#endif

#if defined (__linux__) || defined (linux) || defined (__linux) || defined (__gnu_linux__)
#  define CU_OS_LINUX 1
#else
#  define CU_OS_LINUX 0
#endif

#if defined (__Lynx__)
#  define CU_OS_LYNX 1
#else
#  define CU_OS_LYNX 0
#endif

#if (defined (macintosh) || defined (Macintosh)) && (defined (__APPLE__) || defined (__MACH__))
#  if defined (__APPLE__) && defined (__MACH__)
#    define CU_OS_MAC 10
#  else
#    define CU_OS_MAC 9
#  endif
#else
#  define CU_OS_MAC 0
#endif

#if defined (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) && (defined (__APPLE__) || defined (__MACH__))
#  define CU_OS_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ - 0) * 1000
#else
#  define CU_OS_IOS 0
#endif

#if defined (__OS9000) || defined (_OSK)
#  define CU_OS_MICROWARE9 1
#else
#  define CU_OS_MICROWARE9 0
#endif

#if defined (__minix)
#  define CU_OS_MINIX 1
#else
#  define CU_OS_MINIX 0
#endif

#if defined (__MORPHOS__)
#  define CU_OS_MORPH 1
#else
#  define CU_OS_MORPH 0
#endif

#if defined (mpeix) || defined (__mpexl)
#  define CU_OS_MPEIX 1
#else
#  define CU_OS_MPEIX 0
#endif

#if defined (MSDOS) || defined (__MSDOS__) || defined (_MSDOS) || defined (__DOS__)
#  define CU_OS_MSDOS 1
#else
#  define CU_OS_MSDOS 0
#endif

#if defined (__TANDEM)
#  define CU_OS_NONSTOP 1
#else
#  define CU_OS_NONSTOP 0
#endif

#if defined (__nucleus__)
#  define CU_OS_NUCLEUS 1
#else
#  define CU_OS_NUCLEUS 0
#endif

#if defined (OS2) || defined (_OS2) || defined (__OS2__) || defined (__TOS_OS2__)
#  define CU_OS_OS2 1
#else
#  define CU_OS_OS2 0
#endif

#if defined (__palmos__)
#  define CU_OS_PALM 1
#else
#  define CU_OS_PALM 0
#endif

#if defined (EPLAN9)
#  define CU_OS_PLAN9 1
#else
#  define CU_OS_PLAN9 0
#endif

#if defined (pyr)
#  define CU_OS_PYRAMIDDC 1
#else
#  define CU_OS_PYRAMIDDC 0
#endif

#if defined (__QNX__) || defined (__QNXNTO__) || defined (_NTO_VERSION)
#  include <sys/neutrino.h>
#  define CU_OS_QNX _NTO_VERSION
#  define CU_OS_UNIX_FORCE
#else
#  define CU_OS_QNX 0
#endif

#if defined (sinux)
#  define CU_OS_SINUX 1
#else
#  define CU_OS_SINUX 0
#endif

#if defined (M_I386) || defined (M_XENIX) || defined (_SCO_DS)
#  define CU_OS_OPENSERVER 1
#else
#  define CU_OS_OPENSERVER 0
#endif

#if defined (__SVR4) || defined (__svr4__) || defined (__sysv__) || defined (_SYSTYPE_SVR4)
#  define CU_OS_SVR4 1
#else
#  define CU_OS_SVR4 0
#endif

#if defined (sun) || defined (__sun)
#  if CU_OS_SVR4
#    define CU_OS_SOLARIS 1
#  else
#    define CU_OS_SUN 1
#  endif
#else
#  define CU_OS_SOLARIS 0
#  define CU_OS_SUN 1
#endif

#if defined (UTS)
#  define CU_OS_UTS 1
#else
#  define CU_OS_UTS 0
#endif

#if defined (__VOS__)
#  define CU_OS_VOS __VOS__ 1
#else
#  define CU_OS_VOS __VOS__ 0
#endif

#if defined (__CUOS__)
#  define CU_OS_CUOS 1
#endif

#if defined (__SYLLABLE__)
#  define CU_OS_SYLLABLE 1
#else
#  define CU_OS_SYLLABLE 0
#endif

#if defined (__SYMBIAN32__)
#  define CU_OS_SYMBIAN 1
#  define CU_OS_32BIT 1
#else
#  define CU_OS_SYMBIAN 0
#endif

#if defined (__osf__) || defined (__osf)
#  define CU_OS_OSF 1
#  define CU_OS_64BIT 1
#  define CU_OS_UNIX_FORCE
#else
#  define CU_OS_OSF 0
#endif

#if defined (ultrix) || defined (__ultrix) || defined (__ultrix__) || (defined (unix) && defined (vax))
#  define CU_OS_ULTRIX 1
#else
#  define CU_OS_ULTRIX 0
#endif

#if defined (_CRAY) || defined (__crayx1)
#  define CU_OS_UNICOSMP 1
#else
#  define CU_OS_UNICOSMP 0
#endif

#if defined (_UNICOS) || CU_OS_UNICOSMP
#  define CU_OS_UNICOS _UNICOS
#else
#  define CU_OS_UNICOS 0
#endif

#if defined (CU_OS_UNIX_FORCE) || \
    defined (__unix__) || defined (__unix) || defined (unix) || \
    defined (_POSIX_SOURCE) || defined (_XOPEN_SOURCE) || CU_OS_MAC
#  define CU_OS_UNIX 1
#  if !defined (_GNU_SOURCE)
#    define _GNU_SOURCE
#  endif
#else
#  define CU_OS_UNIX 0
#endif

#if defined (sco) || defined (_UNIXWARE7)
#  define CU_OS_UNIXWARE 1
#else
#  define CU_OS_UNIXWARE 0
#endif

#if defined (_UWIN)
#  define CU_OS_UWIN 1
#else
#  define CU_OS_UWIN 0
#endif

#if defined (VMS) || defined (__VMS) || defined (__VMS_VER)
#  define CU_OS_VMS __VMS_VER
#else
#  define CU_OS_VMS 0
#endif

#if defined (__VXWORKS__) || defined (__vxworks) || defined (_WRS_VXWORKS_MAJOR)
#  include <version.h>
#  define CU_OS_VXWORKS ((_WRS_VXWORKS_MAJOR - 0) * 100) + ((_WRS_VXWORKS_MINOR - 0) * 10) + (_WRS_VXWORKS_MAINT - 0)
#  if defined (__RTP__)
#    define CU_OS_VXWORKS_REALTIME 1
#  else
#    define CU_OS_VXWORKS_REALTIME 0
#  endif
#  if defined (_WRS_KERNEL)
#    define CU_OS_VXWORKS_KERNEL 1
#  else
#    define CU_OS_VXWORKS_KERNEL 0
#  endif
#else
#  define CU_OS_VXWORKS 0
#  define CU_OS_VXWORKS_REALTIME 0
#  define CU_OS_VXWORKS_KERNEL 0
#endif

#if defined (_WIN16) || defined (_WIN32) || defined (_WIN64) || \
    defined (__WIN32__) || defined (_WIN16) || defined (__TOS_WIN__) || \
    defined (__WINDOWS__) || defined (__MINGW32__) || defined (__MINGW64__)
#  define CU_OS_WINDOWS 1
#  define WIN32_LEAN_AND_MEAN
#  if defined (_WIN64)
#    define CU_OS_WIN64 1
#    define CU_OS_WIN32 0
#    define CU_OS_WIN16 0
#    define CU_OS_64BIT 1
#  elif !defined (_WIN16)
#    define CU_OS_WIN64 0
#    define CU_OS_WIN32 1
#    define CU_OS_WIN16 0
#    define CU_OS_32BIT 1
#  else
#    define CU_OS_WIN64 0
#    define CU_OS_WIN32 0
#    define CU_OS_WIN16 1
#    define CU_OS_16BIT 1
#  endif
#else
#  define CU_OS_WINDOWS 0
#  define CU_OS_WIN64 0
#  define CU_OS_WIN32 0
#  define CU_OS_WIN16 0
#endif

#if defined (_WIN32_WCE) || defined (__SCITECH_SNAP__)
#  define CU_OS_WINCE 1
#  define CU_OS_32BIT 1
#else
#  define CU_OS_WINCE 0
#endif

#if defined (_WINDU_SOURCE)
#  define CU_OS_WINDU _WINDU_SOURCE
#else
#  define CU_OS_WINDU 0
#endif

#if defined (__MVS__) || defined (__HOS_MVS__) || defined (__TOS_MVS__)
#  define CU_OS_ZOS 1
#else
#  define CU_OS_ZOS 0
#endif

#if !defined (CU_OS_64BIT)
#  define CU_OS_64BIT 0
#endif
#if !defined (CU_OS_32BIT)
#  define CU_OS_32BIT 0
#endif
#if !defined (CU_OS_16BIT)
#  define CU_OS_16BIT 0
#endif

/* ==========================================================================
 *
 * ----------------------- Architecture identification ----------------------
 *
 * ========================================================================== */

#if defined (__alpha__) || defined (__alpha) || defined (_M_ALPHA)
#  if defined (__alpha_ev6__)
#    define CU_ARCH_ALPHA 600
#  elif defined (__alpha_ev5__)
#    define CU_ARCH_ALPHA 500
#  elif defined (__alpha_ev4__)
#    define CU_ARCH_ALPHA 400
#  else
#    define CU_ARCH_ALPHA 1
#  endif
#else
#  define CU_ARCH_ALPHA 0
#endif

#if defined (__amd64__) || defined (__amd64) || defined (__x86_64__) \
    || defined (__x86_64) || defined (_M_X64) || defined (_M_AMD64)
#  define CU_ARCH_AMD 1
#  define CU_ARCH_64BIT 1
#endif

#if defined (__ARM_ARCH) || defined (__TARGET_ARCH_ARM) || defined (__TARGET_ARCH_THUMB) || \
    defined (_M_ARM) || defined (_M_ARM64) || defined (__ARM_ARCH) || \
    defined (__arm__) || defined (__arm64) || defined (__arm64__) || \
    defined (__thumb__) || defined (__aarch64__) || defined (__AARCH64EL__)
#  if defined (__ARM_ARCH_2__)
#    define CU_ARCH_ARM 200
#  elif defined (__ARM_ARCH_3__) || defined (__ARM_ARCH_3M__)
#    define CU_ARCH_ARM 300
#  elif defined (__ARM_ARCH_4__) || defined (__ARM_ARCH_4T__) || defined (__TARGET_ARM_4T)
#    define CU_ARCH_ARM 400
#  elif defined (__ARM_ARCH_5TE__) || defined (__ARM_ARCH_5TEJ__) || defined (__ARM_ARCH_5T__)
#    define CU_ARCH_ARM 500
#  elif defined (__ARM_ARCH_6__) || defined (__ARM_ARCH_6J__) || \
        defined (__ARM_ARCH_6ZK__) || defined (__ARM_ARCH_6T2__) || \
	defined (__ARM_ARCH_6KZ__) || defined (__ARM_ARCH_6Z__) || \
	defined (__ARM_ARCH_6K__)
#    define CU_ARCH_ARM 600
#  elif defined (__ARM_ARCH_7__) || defined (__ARM_ARCH_7A__) || \
        defined (__ARM_ARCH_7R__) || defined (__ARM_ARCH_7M__) || \
	defined (__ARM_ARCH_7S__)
#    define CU_ARCH_ARM 700
#  elif defined (__AARCH64EL__) || defined (__aarch64__)
#    define CU_ARCH_ARM 800
#    define CU_ARCH_ARM_AARCH64 1
#    define CU_ARCH_64BIT 1
#  else
#    define CU_ARCH_ARM 1
#  endif
#  if !defined (CU_ARCH_ARM_AARCH64)
#    define CU_ARCH_ARM_AARCH64 0
#  endif
#  if defined (__thumb__)
#    define CU_ARCH_ARM_THUMB 1
#  else
#    define CU_ARCH_ARM_THUMB 0
#  endif
#  if !defined (CU_ARCH_64BIT) && (defined (__arm64) || defined (__arm64__) || defined (_M_ARM64))
#    define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_ARM 0
#  define CU_ARCH_ARM_THUMB 0
#  define CU_ARCH_ARM_AARCH64 0
#endif

#if defined (__bfin__) || defined (__BFIN__) || defined (bfin) || defined (BFIN) || defined (__ADSPBLACKFIN__) || defined (__bfin)
#  define CU_ARCH_BLACKFIN 1
#else
#  define CU_ARCH_BLACKFIN 0
#endif

#if defined (__convex__)
#  if defined (__convex_c1__)
#    define CU_ARCH_CONVEX 100
#  elif defined (__convex_c2_)
#    define CU_ARCH_CONVEX 200
#  elif defined (__convex_c32_)
#    define CU_ARCH_CONVEX 320
#  elif defined (__convex_c34_)
#    define CU_ARCH_CONVEX 340
#  elif defined (__convex_c38_)
#    define CU_ARCH_CONVEX 380
#  else
#    define CU_ARCH_CONVEX 1
#  endif
#else
#  define CU_ARCH_CONVEX 0
#endif

#if defined (__e2k__) || defined (__iset__)
#  if defined (__elbrus_16c__)
#    define CU_ARCH_E2K 1600
#  elif defined (__elbrus_12c__)
#    define CU_ARCH_E2K 1200
#  elif defined (__elbrus_2c3__)
#    define CU_ARCH_E2K 230
#  elif defined (__elbrus_8c__)
#    define CU_ARCH_E2K 80
#  else
#    define CU_ARCH_E2K 1
#  endif
#else
#  define CU_ARCH_E2K 0
#endif

#if defined (__epiphany__)
#  define CU_ARCH_EPIP 1
#else
#  define CU_ARCH_EPIP 0
#endif

#if defined (__hppa__) || defined (__hppa) || defined (__HPPA__)
#  if defined (__PA8000__) || defined (__HPPA20__) || defined (__RISC2_0__) || defined (_PA_RISC2_0)
#    define CU_ARCH_HPPA_RISC 200
#  elif defined (__PA7100__) || defined (__HPPA11__) || defined (_PA_RISC1_1)
#    define CU_ARCH_HPPA_RISC 110
#  elif defined (_PA_RISC1_0)
#    define CU_ARCH_HPPA_RISC 100
#  else
#    define CU_ARCH_HPPA_RISC 1
#  endif
#else
#  define CU_ARCH_HPPA_RISC 0
#endif

#if defined (__ia64__) || defined (_IA64) || defined (__IA64__) || defined (__ia64) || defined (_M_IA64) || defined (__itanium__)
#  define CU_ARCH_IA64 1
#  if !defined (CU_ARCH_64BIT)
#    define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_IA64 0
#endif

#if defined (__8080__) || defined (__8085__) || defined (__Z80) || defined (__Z180)
#  if defined (__Z180)
#    define CU_ARCH_X80 1180
#  elif defined (__Z80)
#    define CU_ARCH_X80 180
#  elif defined (__8085__) && defined (__8080__)
#    define CU_ARCH_X80 85
#  else
#    define CU_ARCH_X80 80
#  endif
#else
#  define CU_ARCH_X80 0
#endif

#if defined (__x86_64) || defined (__x86_64__) || defined (_M_X64)
#  define CU_ARCH_X86 700
#  if !defined (CU_ARCH_64BIT)
#    define CU_ARCH_64BIT 1
#  endif
#elif defined (i386) || defined (__i386__) || defined (__i386) || \
    defined (__i486__) || defined (__i586__) || defined (__i686__) || \
    defined (_M_IX86) || defined (_X86_) || defined (__THW_INTEL__) || \
    defined (__I86__) || defined (__INTEL__)
#  if defined (_M_IX86)
#    define CU_ARCH_X86 _M_IX86
#  elif defined (__i686__) || defined (__I86__)
#    define CU_ARCH_X86 600
#  elif defined (__i586__)
#    define CU_ARCH_X86 500
#  elif defined (__i486__)
#    define CU_ARCH_X86 400
#  elif defined (__i386__)
#    define CU_ARCH_X86 300
#  else
#    define CU_ARCH_X86 1
#  endif
#  define CU_ARCH_32BIT 1
#else
#  define CU_ARCH_X86 0
#endif

#if defined (__loongarch__) || defined (__loongarch_arch) || defined (__loongarch_tune)
#  if defined (__loongarch_arch)
#    define CU_ARCH_LOONGARCH __loongarch_arch
#  elif defined (__loongarch_tune)
#    define CU_ARCH_LOONGARCH __loongarch_tune
#  else
#    define CU_ARCH_LOONGARCH 1
#  endif
#else
#  define CU_ARCH_LOONGARCH 0
#endif

#if defined (__m68k__) || defined (M68000) || defined (__MC68K__)
#  if defined (__mc68060__) || defined (__mc68060) || defined (mc68060)
#    define CU_ARCH_M68K 68060
#  elif defined (__mc68040__) || defined (__mc68040) || defined (mc68040)
#    define CU_ARCH_M68K 68040
#  elif defined (__mc68030__) || defined (__mc68030) || defined (mc68030)
#    define CU_ARCH_M68K 68030
#  elif defined (__mc68020__) || defined (__mc68020) || defined (mc68020)
#    define CU_ARCH_M68K 68020
#  elif defined (__mc68010__) || defined (__mc68010) || defined (mc68010)
#    define CU_ARCH_M68K 68010
#  elif defined (__mc68000__) || defined (__mc68000) || defined (mc68000)
#    define CU_ARCH_M68K 68000
#  else
#    define CU_ARCH_M68K 1
#  endif
#  if !defined (CU_ARCH_32BIT)
#    define CU_ARCH_32BIT 1
#  endif
#else
#  define CU_ARCH_M68K 0
#endif

#if defined (__mips__) || defined (__mips) || defined (__MIPS__)
#  if defined (__MIPS_ISA4__) || defined (_MIPS_ISA_MIPS4)
#    define CU_ARCH_MIPS 400
#  elif defined (__MIPS_ISA3__) || defined (_MIPS_ISA_MIPS3)
#    define CU_ARCH_MIPS 300
#  elif defined (__MIPS_ISA2__) || defined (_MIPS_ISA_MIPS2) || defined (_R4000)
#    define CU_ARCH_MIPS 200
#  elif defined (__MIPS_ISA1__) || defined (_MIPS_ISA_MIPS1) || defined (_R3000)
#    define CU_ARCH_MIPS 100
#  else
#    define CU_ARCH_MIPS 1
#  endif
#else
#  define CU_ARCH_MIPS 0
#endif

#if defined (__ve__) || defined (__ve) || defined (__NEC__)
#  define CU_ARCH_NECSX 1
#else
#  define CU_ARCH_NECSX 0
#endif

#if defined (__pnacl__)
#  define CU_ARCH_PNACL 1
#else
#  define CU_ARCH_PNACL 0
#endif

#if defined (__powerpc) || defined (__powerpc__) || defined (__powerpc64__) || \
    defined (_ARCH_PPC) || defined (_ARCH_PPC64) || defined (__PPCGECKO__) || \
    defined (__POWERPC__) || defined (__ppc__) || defined (__ppc64__) || \
    defined (__PPCBROADWAY__) || defined (_XENON) || defined (__ppc) || \
    defined (__PPC__) || defined (__PPC64__) || defined (_M_PPC)
#  if defined (__ppc604__) || defined (_ARCH_604)
#    define CU_ARCH_PPC 604
#  elif defined (__ppc603__) || defined (_ARCH_603)
#    define CU_ARCH_PPC 603
#  elif defined (__ppc601__) || defined (_ARCH_601)
#    define CU_ARCH_PPC 601
#  elif defined (_ARCH_450)
#    define CU_ARCH_PPC 450
#  elif defined (_ARCH_440)
#    define CU_ARCH_PPC 440
#  else
#    define CU_ARCH_PPC 620
#  endif
#  if defined (__powerpc64__) || defined (__ppc64__) || defined (__PPC64__) || defined (_ARCH_PPC64)
#      define CU_ARCH_64BIT 1
#  endif
#else
#  define CU_ARCH_PPC 0
#endif

#if defined (__CUDA_ARCH__)
#  define CU_ARCH_PTX __CUDA_ARCH__
#else
#  define CU_ARCH_PTX 0
#endif

#if defined (pyr)
#  define CU_ARCH_PYRAMID 1
#else
#  define CU_ARCH_PYRAMID 0
#endif

#if defined (__riscv) || defined (__riscv_32) || defined (__riscv_64)
#  if defined (__riscv_64) && !defined (CU_ARCH_64BIT)
#    define CU_ARCH_64BIT 1
#  elif defined (__riscv_32) && !defined (CU_ARCH_32BIT)
#    define CU_ARCH_32BIT 1
#  endif
#  define CU_ARCH_RISCV 1
#else
#  define CU_ARCH_RISCV 0
#endif

#if defined (__THW_RS6000) || defined (_IBMR2) || defined (_POWER) || \
    defined (_ARCH_PWR) || defined (_ARCH_PWR2) || defined (_ARCH_PWR3) || defined (_ARCH_PWR4)
#  define CU_ARCH_RS6000 1
#else
#  define CU_ARCH_RS6000 0
#endif

#if defined (__sparc__) || defined (__sparc)
#  if defined (__sparcv9) || defined (__sparc_v9__)
#    define CU_ARCH_SPARC 900
#  elif defined (__sparcv8) || defined (__sparc_v8__)
#    define CU_ARCH_SPARC 800
#  else
#    define CU_ARCH_SPARC 1
#  endif
#else
#  define CU_ARCH_SPARC 0
#endif

#if defined (__sh__)
#  if defined (__SH5__)
#    define CU_ARCH_SH 500
#  elif defined (__SH4__)
#    define CU_ARCH_SH 400
#  elif defined (__sh3__) || defined (__SH3__)
#    define CU_ARCH_SH 300
#  elif defined (__sh2__)
#    define CU_ARCH_SH 200
#  elif defined (__sh1__)
#    define CU_ARCH_SH 100
#  else
#    define CU_ARCH_SH 1
#  endif
#else
#  define CU_ARCH_SH 0
#endif

#if defined (_TMS320C2XX) || defined (__TMS320C2000__) || \
    defined (_TMS320C5X) || defined (__TMS320C55X__) || \
    defined (_TMS320C6X) || defined (__TMS320C6X__)
#  define CU_ARCH_TMS320 1
#else
#  define CU_ARCH_TMS320 0
#endif

#if defined (__370__) || defined (__THW_370__) || defined (__s390__)
#  if defined (__s390__)
#    define CU_ARCH_SYSZ 390
#  else
#    define CU_ARCH_SYSZ 370
#  endif
#else
#  define CU_ARCH_SYSZ 0
#endif

#if !defined (CU_ARCH_64BIT)
#  define CU_ARCH_64BIT 0
#endif
#if !defined (CU_ARCH_32BIT)
#  define CU_ARCH_32BIT 0
#endif

/* ==========================================================================
 *
 * ------------------------- Platform identification ------------------------
 *
 * ========================================================================== */

#if defined (__ANDROID__)
#  include <android/api-level.h>
#  define CU_PLAT_ANDROID __ANDROID_API__
#else
#  define CU_PLAT_ANDROID 0
#endif

#if defined (__CloudABI__)
#  define CU_PLAT_CLOUDABI 1
#else
#  define CU_PLAT_CLOUDABI 0
#endif

#if defined (TARGET_IPHONE_SIMULATOR) || defined (TARGET_OS_SIMULATOR) || \
    defined (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#  define CU_PLAT_IOS 1
#else
#  define CU_PLAT_IOS 0
#endif

#if defined (__MINGW32__) || defined (__MINGW64__)
#  define CU_PLAT_MINGW 1
#  if defined (__MINGW64__)
#    define CU_PLAT_MINGW64 ((__MINGW64_VERSION_MAJOR - 0) * 100) + (__MINGW64_VERSION_MINOR - 0)
#    define CU_PLAT_MINGW32 0
#  else
#    define CU_PLAT_MINGW64 0
#    define CU_PLAT_MINGW32 ((__MINGW32_VERSION_MAJOR - 0) * 100) + (__MINGW32_VERSION_MINOR - 0)
#  endif
#else
#  define CU_PLAT_MINGW 0
#  define CU_PLAT_MINGW64 0
#  define CU_PLAT_MINGW32 0
#endif

#if defined (WINAPI_FAMILY)
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

/* ==========================================================================
 *
 * ---------------------------- Language features ---------------------------
 *
 * ========================================================================== */

#define CU_LANG_C23 202311L
#define CU_LANG_C17 201710L
#define CU_LANG_C11 201112L
#define CU_LANG_C99 199901L
#define CU_LANG_C95 199409L
#define CU_LANG_C89 198900L
#define CU_LANG_CKR 197200L

#if defined (__STDC__)
#  if defined (__STDC_VERSION__)
#    define CU_LANG_C __STDC_VERSION__
#  else
#    define CU_LANG_C CU_LANG_C89
#  endif
#else
#  define CU_LANG_C CU_LANG_CKR
#endif

#if CU_COMPVER(GNU, 2, 0)
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

#if CU_COMP_MSVC || (CU_COMP_DMC && CU_ARCH_X86)
#  define CU_ASM __asm
#elif CU_COMP_GNU && !CU_COMP_CHIBICC
#  define CU_ASM __asm__
#else
#  define CU_ASM asm
#endif

#if defined (__STDC_THREADS__)
#  define CU_THREAD_C_AVAILABLE 1
#else
#  define CU_THREAD_C_AVAILABLE 0
#endif

#if CU_OS_WINDOWS
#  define CU_THREAD_WIN_AVAILABLE 1
#else
#  define CU_THREAD_WIN_AVAILABLE 0
#endif

#if CU_OS_UNIX || defined (_POSIX_THREADS) || defined (__EMSCRIPTEN_PTHREADS__)
#  define CU_THREAD_POSIX_AVAILABLE 1
#else
#  define CU_THREAD_POSIX_AVAILABLE 0
#endif

#if CU_THREAD_POSIX_AVAILABLE || CU_THREAD_C_AVAILABLE || CU_THREAD_WIN_AVAILABLE
#  define CU_THREAD_ANY_AVAILABLE 1
#else
#  define CU_THREAD_ANY_AVAILABLE 0
#endif

/* ==========================================================================
 *
 * -------------------------- Preprocessor features -------------------------
 *
 * ========================================================================== */

#define CU_CONCAT3_DETAIL(a, b, c) a ## b ## c
#define CU_CONCAT_DETAIL(a, b) a ## b
#define CU_STRINGIFY_DETAIL(a) #a

#define CU_CONCAT3(a, b, c) CU_CONCAT3_DETAIL(a, b, c)
#define CU_CONCAT(a, b) CU_CONCAT_DETAIL(a, b)
#define CU_STRINGIFY(a) CU_STRINGIFY_DETAIL(a)
#define CU_EVAL(a) a

#if defined (__has_include)
#  define CU_HAS_INCLUDE(inc) __has_include(inc)
#  define CU_HAS_INCLUDE_AVAILABLE 1
#else
#  define CU_HAS_INCLUDE(inc) 0
#  define CU_HAS_INCLUDE_AVAILABLE 0
#endif

#if defined (__has_include_next)
#  define CU_HAS_INCLUDE_NEXT(inc) __has_include_next(inc)
#  define CU_HAS_INCLUDE_NEXT_AVAILABLE 1
#else
#  define CU_HAS_INCLUDE_NEXT(inc) 0
#  define CU_HAS_INCLUDE_NEXT_AVAILABLE 0
#endif

#if defined (__has_attribute) && !CU_COMPVERP(IAR, 8, 5, 9)
#  define CU_HAS_ATTRIBUTE(attrib) __has_attribute(attrib)
#  define CU_HAS_ATTRIBUTE_AVAILABLE 1
#else
#  define CU_HAS_ATTRIBUTE(attrib) 0
#  define CU_HAS_ATTRIBUTE_AVAILABLE 0
#endif

#if defined (__has_builtin)
#  define CU_HAS_BUILTIN(builtin) __has_builtin(builtin)
#  define CU_HAS_BUILTIN_AVAILABLE 1
#else
#  define CU_HAS_BUILTIN(builtin) 0
#  define CU_HAS_BUILTIN_AVAILABLE 0
#endif

#if defined (__has_feature)
#  define CU_HAS_FEATURE(feature) __has_feature(feature)
#  define CU_HAS_FEATURE_AVAILABLE 1
#else
#  define CU_HAS_FEATURE(feature) 0
#  define CU_HAS_FEATURE_AVAILABLE 0
#endif

#if defined (__has_extension)
#  define CU_HAS_EXTENSION(ext) __has_extension(ext)
#  define CU_HAS_EXTENSION_AVAILABLE 1
#else
#  define CU_HAS_EXTENSION(ext) 0
#  define CU_HAS_EXTENSION_AVAILABLE 0
#endif

#if defined (__has_declspec_attribute)
#  define CU_HAS_DECLSPEC_ATTRIB(declattrib) __has_declspec_attribute(declattrib)
#  define CU_HAS_DECLSPEC_ATTRIB_AVAILABLE 1
#else
#  define CU_HAS_DECLSPEC_ATTRIB(declattrib) 0
#  define CU_HAS_DECLSPEC_ATTRIB_AVAILABLE 0
#endif

#if defined (__has_warning)
#  define CU_HAS_WARNING(warning) __has_warning(warning)
#  define CU_HAS_WARNING_AVAILABLE 1
#else
#  define CU_HAS_WARNING(warning) 0
#  define CU_HAS_WARNING_AVAILABLE 0
#endif

#if defined (__FILE__)
#  define CU_FILE __FILE__
#  define CU_FILE_AVAILABLE 1
#elif CU_HAS_BUILTIN(__builtin_FILE)
#  define CU_FILE __builtin_FILE()
#  define CU_FILE_AVAILABLE 2
#else
#  define CU_FILE ((const char *)(0))
#  define CU_FILE_AVAILABLE 0
#endif

#if defined (__DATE__)
#  define CU_DATE CU_EVAL(__DATE__)
#  define CU_DATE_AVAILABLE 1
#else
#  define CU_DATE ((const char *)(0))
#  define CU_DATE_AVAILABLE 0
#endif

#if defined (__TIME__)
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

#if defined (__LINE__)
#  define CU_LINE __LINE__
#  define CU_LINE_AVAILABLE 1
#elif CU_HAS_BUILTIN(__builtin_LINE)
#  define CU_LINE __builtin_LINE()
#  define CU_LINE_AVAILABLE 2
#else
#  define CU_LINE 0
#  define CU_LINE_AVAILABLE 0
#endif
#if defined (__COUNTER__)
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
      CU_COMPVER(IAR, 8, 0) || \
      CU_COMPVER(PGI, 18, 4) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(TI, 15, 12) || \
      CU_COMPVER(CRAY, 5, 0) || \
      CU_COMPVERP(TINYC, 0, 9, 17) || \
      CU_COMPVER(SUNPRO, 8, 0) || \
      (CU_COMPVER(IBM, 10, 1) && defined (__C99_PRAGMA_OPERATOR))
#  define CU_PRAGMA(arg) _Pragma(#arg)
#  define CU_PRAGMA_AVAILABLE 1
#else
#  define CU_PRAGMA(arg)
#  define CU_PRAGMA_AVAILABLE 0
#endif

/* ==========================================================================
 *
 * ------------------------------- Endianness -------------------------------
 *
 * ========================================================================== */

#if CU_HAS_INCLUDE(<endian.h>)
#  include <endian.h>
#elif CU_HAS_INCLUDE(<sys/param.h>) && !CU_ARCH_M68K
#  include <sys/param.h>
#endif

#if defined (__BIG_ENDIAN__) || defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || \
    defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__) || (defined (__BYTE_ORDER__) && defined (__ORDER_BIG_ENDIAN__) && \
     __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  define CU_ENDIAN_BIG 1
#  define CU_ENDIAN_LITTLE 0
#elif defined (__LITTLE_ENDIAN__) || defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || \
      defined (__MIPSEL) || defined (__MIPSEL__) || (defined (__BYTE_ORDER__) && defined (__ORDER_LITTLE_ENDIAN__) && \
       __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#  define CU_ENDIAN_BIG 0
#  define CU_ENDIAN_LITTLE 1
#endif

#if !defined (CU_ENDIAN_BIG) && !defined (CU_ENDIAN_LITTLE)
#  define CU_ENDIAN_LITTLE 1
#  define CU_ENDIAN_BIG 1
#endif

#if defined (__FLOAT_WORD_ORDER__) && defined (__ORDER_BIG_ENDIAN__) && __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
#  define CU_ENDIAN_FLT_LITTLE 0
#  define CU_ENDIAN_FLT_BIG 1
#elif defined (__FLOAT_WORD_ORDER__) && defined (__ORDER_LITTLE_ENDIAN__) && __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define CU_ENDIAN_FLT_LITTLE 1
#  define CU_ENDIAN_FLT_BIG 0
#else
#  if CU_ENDIAN_BIG
#    define CU_ENDIAN_FLT_LITTLE 0
#    define CU_ENDIAN_FLT_BIG 1
#  else
#    define CU_ENDIAN_FLT_LITTLE 1
#    define CU_ENDIAN_FLT_BIG 0
#  endif
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

/* ==========================================================================
 *
 * -------------------------------- Keywords --------------------------------
 *
 * ========================================================================== */

#if CU_LANG_C >= CU_LANG_C99
#  define CU_INLINE inline
#  define CU_INLINE_AVAILABLE 1
#elif CU_COMP_GNU || CU_COMPVER(ARM, 6, 2)
#  define CU_INLINE __inline__
#  define CU_INLINE_AVAILABLE 1
#elif CU_COMPVER(MSVC, 12, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVERP(MCST, 1, 25, 10)
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
      CU_COMPVER(PGI, 17, 10) || \
      CU_COMPVER(IAR, 8, 0) || \
      CU_COMPVERP(MCST, 1, 25, 10) || \
      defined (__clang__)
#  define CU_RESTRICT __restrict
#  define CU_RESTRICT_AVAILABLE 1
#else
#  define CU_RESTRICT
#  define CU_RESTRICT_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C11
#  if CU_HAS_INCLUDE(<stdalign.h>)
#    include <stdalign.h>
#  endif
#  if CU_LANG_C >= CU_LANG_C23
#    define CU_ALIGNAS(x) alignas(x)
#    define CU_ALIGNOF(x) alignof(x)
#  else
#    define CU_ALIGNAS(x) _Alignas(x)
#    define CU_ALIGNOF(x) _Alignof(x)
#  endif
#  define CU_ALIGNMENT_AVAILABLE 1
#else
#  define CU_ALIGNAS(x)
#  define CU_ALIGNOF(x)
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

#if CU_LANG_C >= CU_LANG_C23
#  define CU_THREAD_LOCAL thread_local
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C11
#  define CU_THREAD_LOCAL _Thread_local
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_COMPVER(GNU, 2, 0)
#  define CU_THREAD_LOCAL __thread
#  define CU_THREAD_LOCAL_AVAILABLE 1
#elif CU_COMP_MSVC
#  define CU_THREAD_LOCAL __declspec(thread)
#  define CU_THREAD_LOCAL_AVAILABLE 1
#else
#  define CU_THREAD_LOCAL volatile
#  define CU_THREAD_LOCAL_AVAILABLE 0
#endif

#if CU_LANG_C >= CU_LANG_C11 && !defined (__STDC_NO_ATOMICS__)
#  if CU_ARCH_RISCV && CU_COMP_CLANG
#    include <stdint.h>
#  endif
#  include <stdatomic.h>
#  define CU_ATOMIC _Atomic
#  define CU_ATOMIC_AVAILABLE 1
#else
#  define CU_ATOMIC volatile
#  define CU_ATOMIC_AVAILABLE 0
#endif

/* ==========================================================================
 *
 * ------------------------------- Attributes -------------------------------
 *
 * ========================================================================== */

#if CU_COMPVER(GNU, 2, 5) || defined (__TI_GNU_ATTRIBUTE_SUPPORT__)
#  define CU_EXT_ATTRIBS 1
#  if CU_COMPVER(GNU, 11, 1)
#    define CU_ATTRIB_MALLOC_FULL(dealloc_func, dealloc_ind) __attribute__ ((__malloc__(dealloc_func, dealloc_ind)))
#  else
#    define CU_ATTRIB_MALLOC_FULL(dealloc_func, dealloc_ind) CU_ATTRIB_MALLOC
#  endif
#  if CU_HAS_ATTRIBUTE(__hot__) || CU_COMPVER(GNU, 4, 4)
#    define CU_ATTRIB_COLD __attribute__ ((__cold__))
#    define CU_ATTRIB_HOT __attribute__ ((__hot__))
#  else
#    define CU_ATTRIB_COLD
#    define CU_ATTRIB_HOT
#  endif
#  if CU_HAS_ATTRIBUTE(__alloc_size__) || CU_COMPVER(GNU, 4, 4)
#    define CU_ATTRIB_ALLOCSIZE(arg_ind) __attribute__ ((__alloc_size__ (arg_ind)))
#    define CU_ATTRIB_ALLOCSIZE_MUL(arg_ind, arg_ind_p) __attribute__ ((__alloc_size__ (arg_ind, arg_ind_p)))
#  else
#    define CU_ATTRIB_ALLOCSIZE(arg_ind)
#    define CU_ATTRIB_ALLOCSIZE_MUL(arg_ind, arg_ind_p)
#  endif
#  define CU_ATTRIB_ALIGNED(bytes) __attribute__ ((__aligned__ (bytes)))
#  define CU_ATTRIB_ARTIFICIAL __attribute__ ((__artificial__))
#  define CU_ATTRIB_FLATTEN __attribute__ ((__flatten__))
#  define CU_ATTRIB_MALLOC __attribute__ ((__malloc__))
#  define CU_ATTRIB_NOREORDER __attribute__ ((__noreorder__))
#  define CU_ATTRIB_NOINLINE __attribute__ ((__noinline__))
#  if !CU_COMP_TI
#    define CU_ATTRIB_NONNULL(inds) __attribute__ ((__nonnull__ inds))
#  else
#    define CU_ATTRIB_NONNULL(inds)
#  endif
#  define CU_ATTRIB_UNUSED __attribute__ ((__unused__))
#else
#  define CU_EXT_ATTRIBS 0
#  define CU_ATTRIB_ALIGNED(bytes)
#  define CU_ATTRIB_ALLOCSIZE(arg_ind)
#  define CU_ATTRIB_ALLOCSIZE_MUL(arg_ind, arg_ind_p)
#  define CU_ATTRIB_ARTIFICIAL
#  define CU_ATTRIB_COLD
#  define CU_ATTRIB_FLATTEN
#  define CU_ATTRIB_HOT
#  define CU_ATTRIB_MALLOC
#  define CU_ATTRIB_MALLOC_FULL(dealloc_func, dealloc_ind)
#  define CU_ATTRIB_NOREORDER
#  define CU_ATTRIB_NOINLINE
#  define CU_ATTRIB_NONNULL(inds)
#  define CU_ATTRIB_UNUSED
#endif

#if CU_COMPVER(MSVC, 14, 0)
#  define CU_DEPRECATED(msg) __declspec(deprecated(msg))
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_COMPVER(MSVC, 13, 10) || CU_COMPVER(PELLES, 6, 50)
#  define CU_DEPRECATED(msg) __declspec(deprecated)
#  define CU_DEPRECATED_AVAILABLE 1
#elif (CU_HAS_EXTENSION(attribute_deprecated_with_message) && !CU_COMP_IAR) || \
       CU_COMPVER(GNU, 4, 5) || \
       CU_COMPVER(INTEL, 13, 0) || \
       CU_COMPVER(ARM, 5, 6) || \
       CU_COMPVER(SUNPRO, 5, 13) || \
       CU_COMPVER(PGI, 17, 10) || \
       CU_COMPVER(TI, 18, 1) || \
       CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_COMPVER(IAR, 8, 0)
#  define CU_DEPRECATED(msg) _Pragma("deprecated")
#  define CU_DEPRECATED_AVAILABLE 1
#elif CU_HAS_ATTRIBUTE(deprecated) || \
      CU_COMPVER(GNU, 3, 1) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(TI, 15, 12) || \
      CU_COMPVERP(MCST, 1, 25, 10) || \
      CU_COMPVER(IAR, 8, 10)
#  define CU_DEPRECATED(msg) __attribute__((__deprecated__))
#  define CU_DEPRECATED_AVAILABLE 1
#else
#  define CU_DEPRECATED(msg)
#  define CU_DEPRECATED_AVAILABLE 0
#endif

#if CU_HAS_ATTRIBUTE(warn_unused_result) || \
    CU_COMPVER(GNU, 3, 4) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(TI, 15, 12) || \
    CU_COMPVER(PGI, 17, 10) || \
    CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_ATTRIB_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#elif defined (_Check_return_)
#  define CU_ATTRIB_WARN_UNUSED_RESULT _Check_return_
#else
#  define CU_ATTRIB_WARN_UNUSED_RESULT
#endif

#if CU_COMPVER(IAR, 8, 0)
#  define CU_ATTRIB_NORETURN __noreturn
#elif CU_COMPVER(INTEL, 13, 0) || CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_ATTRIB_NORETURN __attribute__((__noreturn__))
#elif CU_LANG_C >= CU_LANG_C11
#  define CU_ATTRIB_NORETURN _Noreturn
#elif CU_HAS_ATTRIBUTE(noreturn) || \
      CU_COMPVER(GNU, 3, 2) || \
      CU_COMPVER(SUNPRO, 5, 11) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 10, 1) || \
      CU_COMPVER(TI, 15, 12) || \
      CU_COMPVER(IAR, 8, 10)
#  define CU_ATTRIB_NORETURN __attribute__((__noreturn__))
#elif CU_COMPVER(SUNPRO, 5, 10)
#  define CU_ATTRIB_NORETURN _Pragma("does_not_return")
#elif CU_COMPVER(MSVC, 13, 10) || CU_COMPVER(PELLES, 9, 0)
#  define CU_ATTRIB_NORETURN __declspec(noreturn)
#elif CU_COMPVER(COMPCERT, 3, 2)
#  define CU_ATTRIB_NORETURN __attribute((noreturn))
#else
#  define CU_ATTRIB_NORETURN
#endif

#if CU_HAS_ATTRIBUTE(pure) || \
    CU_COMPVER(GNU, 2, 9) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1) || \
    CU_COMPVER(TI, 15, 12) || \
    CU_COMPVER(PGI, 17, 10) || \
    CU_COMPVERP(MCST, 1, 25, 10)
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
    CU_COMPVER(IBM, 10, 1) || \
    CU_COMPVER(TI, 15, 12) || \
    CU_COMPVER(PGI, 17, 10) || \
    CU_COMPVERP(MCST, 1, 25, 10)
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
    CU_COMPVER(IBM, 10, 1) || \
    CU_COMPVER(TI, 15, 12) || \
    CU_COMPVERP(MCST, 1, 25, 10) || \
    CU_COMPVER(IAR, 8, 10)
#  define CU_ATTRIB_ALWAYSINLINE __attribute__((__always_inline__)) CU_INLINE
#elif CU_COMPVER(MSVC, 12, 0)
#  define CU_ATTRIB_ALWAYSINLINE __forceinline
#elif CU_COMPVER(IAR, 8, 0)
#  define CU_ATTRIB_ALWAYSINLINE _Pragma("inline=forced")
#else
#  define CU_ATTRIB_ALWAYSINLINE CU_INLINE
#endif

#if CU_HAS_ATTRIBUTE(noinline) || \
    CU_COMPVER(GNU, 4, 0) || \
    CU_COMPVER(INTEL, 13, 0) || \
    CU_COMPVER(SUNPRO, 5, 11) || \
    CU_COMPVER(ARM, 4, 1) || \
    CU_COMPVER(IBM, 10, 1) || \
    CU_COMPVER(TI, 15, 12) || \
    CU_COMPVERP(MCST, 1, 25, 10) || \
    CU_COMPVER(IAR, 8, 10)
#  define CU_ATTRIB_NEVERINLINE __attribute__((__noinline__))
#elif CU_COMPVER(MSVC, 13, 0) || CU_COMPVER(PELLES, 9, 0)
#  define CU_ATTRIB_NEVERINLINE __declspec(noinline)
#elif CU_COMPVER(PGI, 10, 2)
#  define CU_ATTRIB_NEVERINLINE _Pragma("noinline")
#elif CU_COMPVER(COMPCERT, 3, 2)
#  define CU_ATTRIB_NEVERINLINE __attribute((noinline))
#else
#  define CU_ATTRIB_NEVERINLINE
#endif

#if CU_HAS_ATTRIBUTE(nothrow) || CU_COMPVER(GNU, 3, 3) || CU_COMPVER(INTEL, 13, 0) || CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_ATTRIB_NOTHROW __attribute__((__nothrow__))
#elif CU_COMPVER(MSVC, 13, 1) || CU_COMPVER(ARM, 4, 1)
#  define CU_ATTRIB_NOTHROW /* __declspec(nothrow) */
#else
#  define CU_ATTRIB_NOTHROW
#endif

#if CU_HAS_ATTRIBUTE(returns_nonnull) || CU_COMPVER(GNU, 4, 9) || CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_ATTRIB_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#elif defined (_Ret_notnull_)
#  define CU_ATTRIB_RETURNS_NONNULL _Ret_notnull_
#else
#  define CU_ATTRIB_RETURNS_NONNULL
#endif

#if defined (_WIN32) || defined (__CYGWIN__)
#  define CU_VISIBILITY_PRIVATE
#  define CU_VISIBILITY_PUBLIC __declspec(dllexport)
#  define CU_VISIBILITY_IMPORT __declspec(dllimport)
#elif CU_HAS_ATTRIBUTE(visibility) || \
      CU_COMPVER(GNU, 3, 3) || \
      CU_COMPVER(SUNPRO, 5, 11) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 13, 1) || \
      CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_VISIBILITY_PRIVATE __attribute__((__visibility__("hidden")))
#  define CU_VISIBILITY_PUBLIC __attribute__((__visibility__("default")))
#  define CU_VISIBILITY_IMPORT extern
#else
#  define CU_VISIBILITY_PRIVATE
#  define CU_VISIBILITY_PUBLIC
#  define CU_VISIBILITY_IMPORT extern
#endif

#if 0
#  define CU_API CU_VISIBILITY_PUBLIC
#elif 0
#  define CU_API CU_VISIBILITY_IMPORT
#else
#  define CU_API
#endif

#define CU_API_SOURCE CU_API

#if CU_COMPVER(CLANG, 12, 0) || CU_COMPVER(GNU, 7, 0) || (CU_HAS_ATTRIBUTE(__fallthrough__) && !CU_COMP_CLANG) || CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_FALLTHROUGH  ((void)(0)); __attribute__ ((__fallthrough__));
#  define CU_FALLTHROUGH_AVAILABLE 1
#elif defined (__fallthrough)
#  define CU_FALLTHROUGH __fallthrough
#  define CU_FALLTHROUGH_AVAILABLE 1
#elif CU_LANG_C >= CU_LANG_C23
#  define CU_FALLTHROUGH [[__fallthrough__]]
#  define CU_FALLTHROUGH_AVAILABLE 1
#else
#  define CU_FALLTHROUGH ((void)(0))
#  define CU_FALLTHROUGH_AVAILABLE 0
#endif

/* ==========================================================================
 *
 * -------------------------------- Debugging -------------------------------
 *
 * ========================================================================== */

#if (defined (NDEBUG) || (defined (_DEBUG) && !(_DEBUG - 0))) && !CU_SETTING_FORCE_DEBUG
#  define CU_RELEASE 1
#  define CU_DEBUG 0
#else
#  define CU_RELEASE 0
#  define CU_DEBUG 1
#endif

#if CU_LANG_C >= CU_LANG_C23
#  define CU_STATIC_ASSERT(cond, msg) static_assert(cond, msg);
#elif CU_LANG_C >= CU_LANG_C11 && !CU_COMP_CHIBICC
#  define CU_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg);
#elif CU_COMP_MSVC
#  define CU_STATIC_ASSERT(cond, msg) typedef char CU_CONCAT(cu_static_assert_line, CU_EVAL(CU_LINE))[!!(cond) ? 1 : -1];
#else
#  if CU_LINE_AVAILABLE == 1
#    define CU_STATIC_ASSERT(cond, msg) enum { CU_CONCAT(cu_static_assert_line, CU_EVAL(CU_LINE)) = 1 / (!!(cond)) };
#  elif CU_COUNTER_AVAILABLE && CU_LANG_C > CU_LANG_C23
#    define CU_STATIC_ASSERT(cond, msg) enum { CU_CONCAT(cu_static_assert_no_, CU_EVAL(CU_COUNTER)) = 1 / (!!(cond)) };
#  else
#    define CU_STATIC_ASSERT(cond, msg);
#  endif
#endif

#if defined (__STDC_HOSTED__)
#  if (__STDC_HOSTED__ - 0) == 1
#    define CU_FREESTANDING 0
#  else
#    define CU_FREESTANDING 1
#  endif
#else
#  define CU_FREESTANDING 1
#endif

#if CU_HAS_WARNING("-Wunknown-pragmas")
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
#elif CU_COMPVER(INTEL, 13, 0)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("warning(disable:161)")
#elif CU_COMPVER(PGI, 17, 10)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 1675")
#elif CU_COMPVER(GNU, 4, 3)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:4068))
#elif CU_COMPVER(TI, 16, 9)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 163")
#elif CU_COMPVER(IAR, 8, 0)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress=Pe161")
#elif CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 161")
#else
#  define CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
#endif

#if defined (__clang__)
#  define CU_DIAGNOSTICS_PUSH _Pragma("clang diagnostic push")
#  define CU_DIAGNOSTICS_POP _Pragma("clang diagnostic pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(INTEL, 13, 0)
#  define CU_DIAGNOSTICS_PUSH _Pragma("warning(push)")
#  define CU_DIAGNOSTICS_POP _Pragma("warning(pop)")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(GNU, 4, 6)
#  define CU_DIAGNOSTICS_PUSH _Pragma("GCC diagnostic push")
#  define CU_DIAGNOSTICS_POP _Pragma("GCC diagnostic pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_DIAGNOSTICS_PUSH __pragma(warning(push))
#  define CU_DIAGNOSTICS_POP __pragma(warning(pop))
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(ARM, 5, 6)
#  define CU_DIAGNOSTICS_PUSH _Pragma("push")
#  define CU_DIAGNOSTICS_POP _Pragma("pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(TI, 15, 12)
#  define CU_DIAGNOSTICS_PUSH _Pragma("diag_push")
#  define CU_DIAGNOSTICS_POP _Pragma("diag_pop")
#  define CU_DIAGNOSTICS_AVAILABLE 1
#elif CU_COMPVER(PELLES, 2, 90)
#  define CU_DIAGNOSTICS_PUSH _Pragma("warning(push)")
#  define CU_DIAGNOSTICS_POP _Pragma("warning(pop)")
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
#elif CU_COMPVER(CRAY, 5, 0)
#  define CU_MESSAGE(msg) CU_PRAGMA(_CRI message msg)
#elif CU_COMPVER(IAR, 8, 0) || CU_COMPVER(PELLES, 2, 0)
#  define CU_MESSAGE(msg) CU_PRAGMA(message(msg))
#else
#  define CU_MESSAGE(msg)
#endif

#if CU_HAS_WARNING("-Wunknown-pragmas")
#  define CU_WARNING(msg) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS \
CU_PRAGMA(GCC warning msg) \
CU_DIAGNOSTICS_POP
#elif CU_COMPVER(GNU, 4, 8) || CU_COMPVER(PGI, 18, 4) || CU_COMPVER(INTEL, 13, 0)
#  define CU_WARNING(msg) CU_PRAGMA(GCC warning msg)
#elif CU_COMPVER(MSVC, 15, 0)
#  define CU_WARNING CU_PRAGMA(message(msg))
#else
#  define CU_WARNING(msg) CU_MESSAGE(msg)
#endif

#if CU_FREESTANDING && defined (__GNUC__)
#  define CU_BREAKPOINT() __builtin_trap()
#elif defined (__ibmxl__) || defined (__xlC__)
#  include <builtins.h>
#  define CU_BREAKPOINT() __trap(42)
#elif CU_COMP_MSVC || CU_HAS_BUILTIN(__debugbreak) || CU_COMP_INTEL
#  define CU_BREAKPOINT() __debugbreak()
#elif CU_HAS_BUILTIN(__builtin_debugtrap) || (CU_ARCH_ARM_AARCH64 && CU_OS_APPLE)
#  define CU_BREAKPOINT() __builtin_debugtrap()
#elif defined (__ARMCC_VERSION)
#  define CU_BREAKPOINT() __breakpoint(42)
#elif defined (__DMC__) && defined (_M_IX86)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM int 3h; }
#elif CU_ARCH_X86
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__("int3"); }
#elif CU_ARCH_ARM_THUMB
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__(".inst 0xde01"); }
#elif CU_ARCH_ARM_AARCH64
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__(".inst 0xd4200000"); }
#elif CU_ARCH_ARM
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__(".inst 0xe7f001f0"); }
#elif defined (__alpha__) && !defined (__osf__)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__("bpt"); }
#elif defined (_54_)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__("ESTOP"); }
#elif defined (_55_)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
#elif defined (_64P_)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__("SWBP 0"); }
#elif defined (_6x_)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)  { CU_ASM __volatile__("NOP\n .word 0x10000000"); }
#elif CU_OS_UNIX
#  include <signal.h>
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void)
{
#if defined (SIGTRAP)
	raise(SIGTRAP);
#else
	raise(SIGABRT);
#endif
}
#else
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED static void __cu_breakpoint(void) { *(volatile char *)0 = 0; }
#endif

#if !defined (CU_BREAKPOINT)
#  define CU_BREAKPOINT() __cu_breakpoint()
#endif

#if CU_HAS_BUILTIN(__builtin_trap)
#  define CU_TRAP() __builtin_trap()
#elif CU_HAS_INCLUDE(<stdlib.h>)
#  define CU_TRAP() abort()
#endif

#if !defined (CU_TRAP)
#  if CU_OS_UNIX
#    include <signal.h>
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED CU_ATTRIB_NORETURN static void __cu_trap(void) { signal(SIGABRT, SIG_DFL); while (1) raise(SIGABRT); }
#  else
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED CU_ATTRIB_NORETURN static void __cu_trap(void) { while (1) *(volatile char *)0 = 0; }
#  endif
#  define CU_TRAP() __cu_trap()
#endif

#if CU_RELEASE
#  define CU_ASSERT(cond)
#elif CU_HAS_INCLUDE(<assert.h>)
#  include <assert.h>
#  define CU_ASSERT(cond) assert(cond)
#elif CU_SETTING_ALLOC_FUNCS
#  define CU_ASSERT(cond) do { \
	if (!(CU_LIKELY((cond)))) cu_deballoc_assert_fail(CU_LINE, CU_FUNC, CU_FILE, CU_STRINGIFY(cond)); \
} while (0)
#else
#  define CU_ASSERT(cond) do { if (!(CU_LIKELY((cond)))) CU_BREAKPOINT(); } while (0)
#endif

#define CU_UNUSED(var) ((void)(var))
#define CU_EMPTY() ((void)(0))

/* ==========================================================================
 *
 * ------------------------------- Data model -------------------------------
 *
 * ========================================================================== */

#if (defined (__fourbyteints__) && !(__fourbyteints__ - 0))
#  define CU_DM_LP32 1
#else
#  define CU_DM_LP32 0
#endif

#if defined (_ILP32) || defined (__ILP32__) || (defined (__fourbyteints__) && __fourbyteints__)
#  define CU_DM_ILP32 1
#else
#  define CU_DM_ILP32 0
#endif

#if defined (_LP64) || defined (__LP64__)
#  define CU_DM_LP64 1
#else
#  define CU_DM_LP64 0
#endif

#if defined (__LLP64__)
#  define CU_DM_LLP64 1
#else
#  define CU_DM_LLP64 0
#endif

#if CU_DM_LLP64 || CU_DM_LP64 || CU_ARCH_64BIT || CU_OS_64BIT
#  define CU_DM_64BIT 1
#  define CU_DM_32BIT 0
#  define CU_DM_16BIT 0
#elif CU_OS_16BIT
#  define CU_DM_64BIT 0
#  define CU_DM_32BIT 0
#  define CU_DM_16BIT 1
#else
#  define CU_DM_64BIT 0
#  define CU_DM_32BIT 1
#  define CU_DM_16BIT 0
#endif

#if CU_DM_LLP64 || CU_COMP_MSVC || CU_COMP_MINGW || CU_ARCH_M68K || CU_ARCH_MIPS
#  define CU_DM_LONGSUF ll
#  define CU_DM_LL 1
#else
#  define CU_DM_LONGSUF l
#  define CU_DM_LL 0
#endif

#if defined (CHAR_BIT)
#  define CU_DM_BYTESIZE CHAR_BIT
#elif defined (__CHAR_BIT__)
#  define CU_DM_BYTESIZE __CHAR_BIT__
#else
#  define CU_DM_BYTESIZE 8
#endif

/* ==========================================================================
 *
 * ---------------------------------- Types ---------------------------------
 *
 * ========================================================================== */

#if CU_LANG_C < CU_LANG_C23
#  if CU_LANG_C >= CU_LANG_C99 && CU_HAS_INCLUDE(<stdbool.h>)
#    include <stdbool.h>
#  else
#    define bool char
#    define true 1
#    define false 0
#  endif
#endif

typedef signed char i8;
typedef signed short i16;
typedef unsigned char u8;
typedef unsigned short u16;

typedef unsigned short ushort;
typedef unsigned int uint;

#define CU_ENUM_MAX 0x7FFFFFFF

#if CU_HAS_INCLUDE(<stdint.h>) || CU_COMPVER(GNU, 4, 5) || defined (_STDINT) || defined (_STDINT_H) || defined (_STDINT_H_) || defined (_STDINT_H_INCLUDED)
#  include <stdint.h>
   typedef int32_t i32;
   typedef int64_t i64;
   typedef uint32_t u32;
   typedef uint64_t u64;
#  define CU_INT_USEINTPTR
#elif defined (__INT32_TYPE__) && defined (__UINT32_TYPE__) && defined (__INT64_TYPE__) && defined (__UINT64_TYPE__)
   typedef __INT32_TYPE__ i32;
   typedef __INT64_TYPE__ i64;
   typedef __UINT32_TYPE__ u32;
   typedef __UINT64_TYPE__ u64;
#elif CU_OS_VMS || CU_COMP_SGI
#  include <inttypes.h>
   typedef int32_t i32;
   typedef int64_t i64;
   typedef uint32_t u32;
   typedef uint64_t u64;
#elif CU_COMP_MSVC
   typedef __int32 i32;
   typedef __int64 i64;
   typedef unsigned __int32 u32;
   typedef unsigned __int64 u64;
#elif CU_COMP_CC65
   typedef signed long int i32;
   typedef signed long int i64;
   typedef unsigned long int u32;
   typedef unsigned long int u64;
#else
#  if CU_HAS_INCLUDE(<limits.h>)
#    include <limits.h>
#    define CU_INT_LIMITS
#    if defined (UINT_MAX) && UINT_MAX == 0xFFFFFFFF
       typedef unsigned int u32;
#    elif defined (ULONG_MAX) && ULONG_MAX == 0xFFFFFFFF
       typedef unsigned long int u32;
#    elif defined (ULLONG_MAX) && ULLONG_MAX == 0xFFFFFFFF
       CU_GNU_EXT typedef unsigned long long int u32;
#    else
#      define CU_INT_LIMITS_NO_U32
#    endif
#    if defined (UINT_MAX) && UINT_MAX == 0xFFFFFFFFFFFFFFFF
       typedef unsigned int u64;
#    elif defined (ULONG_MAX) && ULONG_MAX == 0xFFFFFFFFFFFFFFFF
       typedef unsigned long int u64;
#    elif defined (ULLONG_MAX) && ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
       CU_GNU_EXT typedef unsigned long long int u64;
#      define CU_LLONG_64BIT 1
#    else
#      define CU_INT_LIMITS_NO_U64
#    endif
#    if defined (INT_MAX) && INT_MAX == 0x7FFFFFFF
       typedef signed int i32;
#    elif defined (LONG_MAX) && LONG_MAX == 0x7FFFFFFF
       typedef signed long int i32;
#    elif defined (LLONG_MAX) && LLONG_MAX == 0x7FFFFFFF
       CU_GNU_EXT typedef signed long long int i32;
#    else
#      define CU_INT_LIMITS_NO_I32
#    endif
#    if defined (INT_MAX) && INT_MAX == 0x7FFFFFFFFFFFFFFF
       typedef signed int i64;
#    elif defined (LONG_MAX) && LONG_MAX == 0x7FFFFFFFFFFFFFFF
       typedef signed long int i64;
#    elif defined (LLONG_MAX) && LLONG_MAX == 0x7FFFFFFFFFFFFFFF
       CU_GNU_EXT typedef signed long long int i64;
#    else
#      define CU_INT_LIMITS_NO_I64
#    endif
#  else
#    define CU_INT_LIMITS_NO_U32
#    define CU_INT_LIMITS_NO_U64
#    define CU_INT_LIMITS_NO_I32
#    define CU_INT_LIMITS_NO_I64
#  endif
#  if !defined (CU_INT_LIMITS) || defined (CU_INT_LIMITS_NO_U32) || \
       defined (CU_INT_LIMITS_NO_U64) || defined (CU_INT_LIMITS_NO_I32) || \
       defined (CU_INT_LIMITS_NO_I64)
#    if CU_COMP_TENDRA || CU_COMP_CC65 || CU_DM_LP64
#      if defined (CU_INT_LIMITS_NO_I32)
         typedef signed int i32;
#      endif
#      if defined (CU_INT_LIMITS_NO_I64)
         typedef signed long int i64;
#      endif
#      if defined (CU_INT_LIMITS_NO_U32)
         typedef unsigned int u32;
#      endif
#      if defined (CU_INT_LIMITS_NO_U64)
       typedef unsigned long int u64;
#      endif
#    else
#      if defined (CU_INT_LIMITS_NO_I32)
         typedef signed long i32;
#      endif
#      if defined (CU_INT_LIMITS_NO_I64)
         CU_GNU_EXT typedef signed long long int i64;
#      endif
#      if defined (CU_INT_LIMITS_NO_U32)
         typedef unsigned long u32;
#      endif
#      if defined (CU_INT_LIMITS_NO_U64)
         CU_GNU_EXT typedef unsigned long long int u64;
#        define CU_LLONG_64BIT 1
#      endif
#    endif
#  endif
#endif

#if defined (CU_INT_USEINTPTR) && defined (__SIZEOF_POINTER__) && \
    defined (__SIZEOF_LONG__) && __SIZEOF_POINTER__ > __SIZEOF_LONG__
   typedef intptr_t iptr;
   typedef uintptr_t uptr;
   typedef intmax_t imax;
   typedef uintmax_t umax;
#  define CU_PTR_BYTES __SIZEOF_POINTER__
#else
#  if CU_DM_64BIT
     typedef i64 iptr;
     typedef u64 uptr;
#    define CU_PTR_BYTES 8
#  elif CU_DM_32BIT
     typedef i32 iptr;
     typedef u32 uptr;
#    define CU_PTR_BYTES 4
#  else
     typedef i16 iptr;
     typedef u16 uptr;
#    define CU_PTR_BYTES 2
#  endif
   typedef i64 imax;
   typedef u64 umax;
#endif

#if CU_DM_LL || defined (CU_LLONG_64BIT)
#  define CU_U64_C(a) a ## ULL
#  define CU_I64_C(a) a ## LL
#  define CU_U64_FMT "llu"
#  define CU_I64_FMT "ll"
#else
#  define CU_U64_C(a) a ## UL
#  define CU_I64_C(a) a ## L
#  define CU_U64_FMT "lu"
#  define CU_I64_FMT "l"
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

#if CU_LANG_C >= CU_LANG_C23
#  include <limits.h>
#  define CU_BITINT_MAX BITINT_MAXWIDTH
#  define CU_BITINT(bits) _BitInt(bits)
#  define CU_UBITINT(bits) unsigned _BitInt(bits)
#  define CU_BITINT_AVAILABLE 1
#else
#  define CU_BITINT_MAX 64
#  define CU_BITINT(bits) imax
#  define CU_UBITINT(bits) umax
#  define CU_BITINT_AVAILABLE 0
#endif

#if (CU_LANG_C >= CU_LANG_C99) && !defined (__STDC_NO_COMPLEX__) && (defined (__STDC_IEC_559_COMPLEX__) || \
     defined (__STDC_IEC_60559_COMPLEX__) || defined (_Imaginary_I))
#  include <complex.h>
#  define CU_IMAGINARY_OF(x) (cimag(x))
#  define CU_REAL_OF(x) (creal(x))
#  define CU_COMPLEX complex
#  define CU_COMPLEX_AVAILABLE 1
#elif defined (__GNUC__)
#  define CU_COMPLEX __complex__
#  define CU_IMAGINARY_OF(x) (__imag__ x)
#  define CU_REAL_OF(x) (__real__ x)
#  define CU_COMPLEX_AVAILABLE 1
#else
#  define CU_COMPLEX
#  define CU_IMAGINARY_OF(x)
#  define CU_REAL_OF(x)
#  define CU_COMPLEX_AVAILABLE 0
#endif

#if defined (INT128_MAX) || defined (__SIZEOF_INT128__)
#  define CU_INT128_AVAILABLE 1
   CU_GNU_EXT typedef __int128 imaxl;
   CU_GNU_EXT typedef unsigned __int128 umaxl;
#else
#  define CU_INT128_AVAILABLE 0
   typedef imax imaxl;
   typedef umax umaxl;
#endif

#if CU_LANG_C >= CU_LANG_C23 && !CU_COMP_MRISC && defined (__STDC_IEC_60559_DFP__)
   typedef _Decimal32 real32;
   typedef _Decimal64 real64;
   typedef _Decimal128 real128;
#  define CU_DECIMAL128_AVAILABLE 1
#elif CU_COMP_SDCC
   typedef float real32;
   typedef float real64;
   typedef float real128;
#  define CU_DECIMAL128_AVAILABLE 0
#elif CU_COMP_CC65
   typedef float real32;
   typedef double real64;
   typedef double real128;
#  define CU_DECIMAL128_AVAILABLE 0
#else
   typedef float real32;
   typedef double real64;
   typedef long double real128;
#  if CU_DM_64BIT && (CU_OS_HPUX || CU_ARCH_SPARC || CU_ARCH_MIPS || CU_ARCH_ARM || CU_OS_ZOS)
#    define CU_DECIMAL128_AVAILABLE 1
#  else
#    define CU_DECIMAL128_AVAILABLE 0
#  endif
#endif

CU_STATIC_ASSERT(sizeof(i32) == 4, "Invalid type size (i32)")
#if !CU_COMP_CC65
CU_STATIC_ASSERT(sizeof(i64) == 8, "Invalid type size (i64)")
#endif
CU_STATIC_ASSERT(sizeof(iptr) >= sizeof(void *), "Invalid type size (iptr)")
CU_STATIC_ASSERT(sizeof(imax) >= sizeof(void *), "Invalid type size (imax)")

/* ==========================================================================
 *
 * ------------------------------ Optimization ------------------------------
 *
 * ========================================================================== */

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
     CU_COMPVER(PGI, 18, 10) || \
     CU_COMPVER(INTEL, 13, 0) || \
     CU_COMPVERP(IBM, 13, 1, 5) || \
     CU_COMPVER(CRAY, 10, 0) || \
     CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_UNREACHABLE() __builtin_unreachable()
#elif defined (CU_ASSUME)
#  define CU_UNREACHABLE() CU_ASSUME(0)
#else
#  define CU_UNREACHABLE()
#endif
#if !defined (CU_ASSUME)
#  define CU_ASSUME(expr) ((void)((expr) ? 1 : (CU_UNREACHABLE(), 1)))
#endif

#if (CU_HAS_BUILTIN(__builtin_expect_with_probability) && !CU_COMP_PGI) || \
     CU_COMPVER(GNU, 9, 0) || CU_COMPVERP(MCST, 1, 25, 10)
#  define CU_PREDICT(expr, result, probability) __builtin_expect_with_probability((expr), (result), (probability))
#  define CU_LIKELY(expr) __builtin_expect(!!(expr), 1)
#  define CU_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#elif CU_HAS_BUILTIN(__builtin_expect) || \
      CU_COMPVER(GNU, 3, 0) || \
      CU_COMPVER(INTEL, 13, 0) || \
      CU_COMPVER(ARM, 4, 1) || \
      CU_COMPVER(IBM, 10, 1) || \
      CU_COMPVER(TI, 15, 12) || \
      CU_COMPVERP(TINYC, 0, 9, 27) || \
      CU_COMPVER(CRAY, 8, 1) || \
      CU_COMPVERP(MCST, 1, 25, 10)
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

/* ==========================================================================
 *
 * ---------------------------- Bit manipulation ----------------------------
 *
 * ========================================================================== */

#if CU_COMP_CHIBICC || CU_COMP_CC65 || CU_COMP_MSVC || CU_COMP_NORCROFT || CU_COMP_TI || !CU_HAS_BUILTIN(__builtin_ffs)
#  define CU_BUILTIN_BITOPS 0
#else
#  define CU_BUILTIN_BITOPS 1
#endif

#if CU_BUILTIN_BITOPS
#  define CU_BITOP_FUNC(n, f, t) CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED CU_ATTRIB_NOTHROW CU_ATTRIB_CONST CU_ATTRIB_WARN_UNUSED_RESULT \
   static int cu_bitop_##f(t x) { return CU_CONCAT(__builtin_##n, CU_DM_LONGSUF)(x); }

   CU_BITOP_FUNC(ffs, ffs, i64)
   CU_BITOP_FUNC(clz, clz, u64)
   CU_BITOP_FUNC(ctz, ctz, u64)
   CU_BITOP_FUNC(clrsb, clrsb, i64)
   CU_BITOP_FUNC(popcount, popcount, u64)
   CU_BITOP_FUNC(parity, parity, u64)
#else
#  define CU_BITOP_DEFN(n) \
   CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_UNUSED CU_ATTRIB_NOTHROW \
   CU_ATTRIB_CONST CU_ATTRIB_WARN_UNUSED_RESULT static int cu_bitop_##n

CU_BITOP_DEFN(INTERNAL_lzcnt)(u64 x)
{
	int bits;
	for (bits = sizeof x * CU_DM_BYTESIZE; x; --bits, x = CU_DWSHIFT(x, 1));
	return bits;
}
CU_BITOP_DEFN(ffs)(i64 x)
{
	int bits;
	if (!x) return 0;
	for (bits = 1; !(x & 1); ++bits, x = CU_DWSHIFT(x, 1));
	return bits;
}
CU_BITOP_DEFN(clz)(u64 x) { return cu_bitop_INTERNAL_lzcnt(x); }
CU_BITOP_DEFN(ctz)(u64 x)
{
	int bits;
	if (!x) return 0;
	for (bits = 0; !(x & 1); ++bits, x = CU_DWSHIFT(x, 1));
	return bits;
}
CU_BITOP_DEFN(clrsb)(i64 x) { return (cu_bitop_INTERNAL_lzcnt((u64)(CU_DWSHIFT(x, sizeof x * 8 - 1) ^ x))) - 1; }
CU_BITOP_DEFN(popcount)(u64 x)
{
	int bits;
	for (bits = 0; x; ++bits, x &= x - 1);
	return bits;
}
CU_BITOP_DEFN(parity)(u64 x) { return cu_bitop_popcount(x) & 1; }

#endif

#define CU_BITOF(x, bit) (CU_DWSHIFT(x, bit) & 1)
#define CU_BITSOF(x, start, end) (CU_DWSHIFT(x, start) & (CU_UPSHIFT(1, (1 + (end - start))) - 1))

/* ==========================================================================
 *
 * -------------------------------- Constants -------------------------------
 *
 * ========================================================================== */

#define CU_PI     3.141592653589793238462643383279502884197169399375105820974944
#define CU_SQRT2  1.414213562373095048801688724209698078569671875376948073176679
#define CU_SQRT3  1.732050807568877293527446341505872366942805253810380628055806
#define CU_EULER  2.718281828459045235360287471352662497757247093699959574966967

#define CU_TODEG_MULT (180.0 / CU_PI)
#define CU_TORAD_MULT (CU_PI / 180.0)

#define CU_I8MAX 0x7F
#define CU_U8MAX 0xFF
#define CU_I16MAX 0x7FFF
#define CU_U16MAX 0xFFFF
#define CU_I32MAX 0x7FFFFFFF
#define CU_U32MAX 0xFFFFFFFF
#define CU_I64MAX 0x7FFFFFFFFFFFFFFF
#define CU_U64MAX 0xFFFFFFFFFFFFFFFF

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

#if CU_PTR_BYTES == 8
#  define CU_UPTRMAX CU_U64MAX
#  define CU_IPTRMAX CU_U64MAX
#elif CU_PTR_BYTES == 4
#  define CU_UPTRMAX CU_U32MAX
#  define CU_IPTRMAX CU_I32MAX
#elif CU_PTR_BYTES == 2
#  define CU_UPTRMAX CU_U16MAX
#  define CU_IPTRMAX CU_I16MAX
#endif

#if defined (PATH_MAX)
#  define CU_PATH_MAX PATH_MAX
#elif CU_OS_WINDOWS
#  define CU_PATH_MAX 520
#elif CU_OS_MAC || CU_OS_BSD
#  define CU_PATH_MAX 1024
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

#if !defined (NULL)
#  define NULL ((void *)(0))
#endif

/* ==========================================================================
 *
 * ---------------------------- Function support ----------------------------
 *
 * ========================================================================== */

#if !CU_SETTING_QUIET

/* Define CU_SETTING_QUIET to 1 before including this header file or as part of your compiler options to disable support warnings. */

#if CU_SETTING_FILE_FUNCS && !CU_OS_WINDOWS && (!CU_HAS_INCLUDE(<dirent.h>) || CU_ARCH_M68K || defined (__MSP430__))
CU_WARNING("Recursive directory deletion is unavailable.")
#endif

#if CU_SETTING_FILE_FUNCS && !CU_HAS_INCLUDE(<sys/stat.h>)
CU_WARNING("File functions are unavailable.")
#  undef CU_SETTING_FILE_FUNCS
#  define CU_SETTING_FILE_FUNCS 0
#endif

#if CU_SETTING_RESOURCES_FUNCS && !CU_HAS_INCLUDE(<cpuid.h>) && !CU_COMP_MSVC
CU_WARNING("cpuid instruction not fully supported so cu_res_cpuinfo will be limited.")
#endif

#if CU_SETTING_NETWORK_FUNCS && !CU_OS_WINDOWS && !CU_OS_UNIX
CU_WARNING("Networking functions are unavailable.")
#  undef CU_SETTING_NETWORK_FUNCS
#  define CU_SETTING_NETWORK_FUNCS 0
#endif

#if CU_SETTING_THREAD_FUNCS && !CU_THREAD_ANY_AVAILABLE
CU_WARNING("Threading functions are unavailable.")
#  undef CU_SETTING_THREAD_FUNCS
#  define CU_SETTING_THREAD_FUNCS 0
#endif

#if CU_SETTING_THREAD_FUNCS && CU_OS_UNIX && !defined(SYS_gettid)
#  if CU_HAS_INCLUDE(<sys/syscall.h>)
#    include <sys/syscall.h>
#  endif
#  if !defined (SYS_gettid)
CU_WARNING("cu_thread_gettid will always return 0.")
#  endif
#endif

#endif

#if CU_SETTING_FUNCS

/* ==========================================================================
 *
 * ---------------------------- String functions ----------------------------
 *
 * ========================================================================== */

typedef struct custr
{
	char *str;
	uptr len, cap;
} custr;

#define CUSTR_EMPTY { NULL, 0, 0 }

/* Creates a custr from a normal string.
   The given custr pointer should be uninitialized or, if it contains allocated text, cleared with custr_clear. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int custr_create(custr *CU_RESTRICT c, const char *CU_RESTRICT str);
/* Creates a custr using an already allocated string; returns the given custr.
   You can provide the allocated size or 0 to assume it is strlen + 1.
   The given custr pointer should be uninitialized or, if it contains allocated text, cleared with custr_clear.
   The allocated string pointer should not be used directly. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API custr *custr_allocd(custr *CU_RESTRICT c, const char *CU_RESTRICT allocdstr, uptr allocd_bytes);
/* Returns a temporary custr for use in reasonable custr functions.
   No cleanup or deallocations are needed: can pass in directly as the argument. */
CU_ATTRIB_NOTHROW
CU_API custr *custr_c(const char *tmpstr);
/* Same as custr_c, but creates a temporary string from a given character. */
CU_ATTRIB_NOTHROW
CU_API custr *custr_char(char chrstr);

/* Changes reserved space of custr and returns whether it succeeded.
   0 bytes deallocates it and will always succeed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_reserve(custr *c, uptr bytes);
/* Copies a custr to another. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int custr_copy(const custr *CU_RESTRICT copy, custr *CU_RESTRICT paste);

/* Sets custr length and moves terminator. Fails if given length is not smaller than current length. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_shrinkto(custr *c, uptr shrinked_len);
/* Deallocates a custr's data and returns it. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API custr *custr_clear(custr *c);
/* Optimizes the capacity of the given custr. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_optimize(custr *c);

/* Inserts 'to_insert' into 'c' at their respective offsets. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((3))
CU_API int custr_insert(custr *CU_RESTRICT c, uptr c_offset, const custr *CU_RESTRICT to_insert, uptr to_insert_offset);
/* Same as custr_insert but 'c_offset' is the length of 'c'. */
CU_ATTRIB_NOTHROW
CU_API int custr_append(custr *c, const custr *to_append, uptr to_append_offset);

/* Gets a substring of a given custr (inclusive indices). The ending index is clamped to the last character.
   The substring should not be allocated beforehand. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int custr_sub(const custr *CU_RESTRICT c, custr *CU_RESTRICT subresult, uptr start_ind, uptr end_ind);
/* Sets the given custr to a substring of itself. The ending index is clamped to the last character. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_tosub(custr *c, uptr start_ind, uptr end_ind);
/* Removes a specified section of the given custr. count is clamped if it would go past the end.
   No effect if starting index is past the end of the string. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void custr_cut(custr *c, uptr start_ind, uptr count);

/* Returns the number of occurrences of a character in a custr. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_count(const custr *c, char target);
/* Returns the number of occurrences of a substring in a custr. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int custr_countsub(const custr *CU_RESTRICT c, const custr *CU_RESTRICT target);

/* Sets the given custr to describe a variadically formated string.
   The custr should not be allocated beforehand.
   Support for va_copy and snprintf is required. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
#if CU_COMPVER(GNU, 2, 0)
__attribute__((__format__ (printf, 2, 3)))
#endif
CU_API int custr_fmt(custr *CU_RESTRICT c, char *CU_RESTRICT fmt, ...);

/* Appends the given file or directory name to 'c' (assuming it is a path).
   If given name is NULL, 'c' will be changed to describe the parent directory instead. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int custr_cd(custr *CU_RESTRICT c, const custr *CU_RESTRICT name);
/* Simplify the given path, assuming 'c' describes one. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void custr_simplify(custr *c);

/* Returns the index of the nth occurrence of a character in a custr, or CU_UPTRMAX if not found.
   n =  0,  1 returns the first and second occurrence respectively.
   n = -1, -2 returns the last and second last occurrence respectively.
   If searching backwards (negative n), the offset will also apply backwards. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API uptr custr_find(const custr *c, uptr c_offset, char target, int n);
/* Same as custr_find, but searches for any character that is NOT target. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API uptr custr_findnot(const custr *c, uptr c_offset, char target, int n);

/* Returns the index of the first character of the nth occurrence of a substring in a custr, or CU_UPTRMAX if not found.
   n =  0,  1 returns the first and second occurrence respectively.
   n = -1, -2 returns the last and second last occurrence respectively.
   If searching backwards (negative n), the offset will also apply backwards. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 3))
CU_API uptr custr_findsub(const custr *CU_RESTRICT c, uptr c_offset, const custr *CU_RESTRICT target_substr, int n);

/* Replaces a character with another. Null terminator replacement removes all occurrences.
   No effect if target character is a null terminator. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void custr_replace(custr *c, uptr c_offset, char target, char replacement);

/* Replaces a substring with another. A replacement of a null pointer or null custr removes all occurrences.
   No effect if target substring is a null or empty string. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 3))
CU_API int custr_replacesub(custr *CU_RESTRICT c, uptr c_offset, const custr *CU_RESTRICT target, const custr *replacement);

/* ==========================================================================
 *
 * -------------------------- Filesystem functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_FILE_FUNCS

typedef struct cu_file_info
{
	i64 fsize_bytes; /* file bytes size */
	i64 mod_time;    /* last modification time */
	i64 access_time; /* last access time */
} cu_file_info;

/* Returns whether the path is a file. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_file_exists(const char *path);
/* Returns whether the path is a directory. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_dir_exists(const char *path);

/* Create a directory. Succeeds if it already exists. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_dir_create(const char *path);

/* Read a number of bytes from a file.
   'bytes' determines how many bytes of the given file are read into 'result'.
   It is clamped to the file's size. Afterwards, 'bytes' is changed to the file size.
   If 'bytes' is a NULL pointer or zero, the entire file is read into 'result'.
   If 'result' is NULL, enough data is allocated to store the file contents.
   Returns the given 'result' pointer/allocated data, or NULL on error. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void *cu_file_read(const char *CU_RESTRICT path, void *CU_RESTRICT result, int binary_file, uptr *CU_RESTRICT bytes);

/* Write or append a number of bytes of 'content' into a file.
   Mode values can be found in the macros CU_FILE_(WRITE/APPEND)(TXT/BIN). */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int cu_file_write(const char *CU_RESTRICT path, const void *CU_RESTRICT content, unsigned int mode, uptr bytes);

#define CU_FILE_WRITETXT 0
#define CU_FILE_WRITEBIN 1
#define CU_FILE_APPENDTXT 2
#define CU_FILE_APPENDBIN 3

/* Gets file information. Fails if file does not exist. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API int cu_file_getinfo(const char *CU_RESTRICT path, cu_file_info *CU_RESTRICT f_info);

/* Determines the current running executable's path.
   Returns allocated string and changes 'len' to allocated bytes.
   On error, NULL is returned and passed length pointer is unaffected. */
CU_ATTRIB_NOTHROW
CU_API char *cu_file_exe_path(const char *CU_RESTRICT argv_first, uptr *CU_RESTRICT allocated);

/* Removes the file at the given path. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_file_delete(const char *path);
/* Removes the directory at the given path. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_dir_delete(const char *path, int recursive);

#endif

/* ==========================================================================
 *
 * ------------------------------ RNG functions -----------------------------
 *
 * ========================================================================== */

#if CU_SETTING_RAND_FUNCS

/* Get cryptographically secure random bytes if available.
   Returns number of random bytes written. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API uptr cu_rand_cryptographic(void *data, uptr bytes);

/* Quickly get deterministic random unsigned 64 bit value. */
CU_ATTRIB_NOTHROW
CU_API u64 cu_rand(void);
/* Set the seed for cu_rand. */
CU_ATTRIB_NOTHROW
CU_API void cu_srand(u64 seed);

#define CU_RND_STATE_SZ 312

/* Mersenne twister RNG state. */
typedef struct cu_rand_state
{
	i64 index;
	u64 state[312];
} cu_rand_state;

/* Initialize random state. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_rand_init(cu_rand_state *state, u64 seed);
/* Returns a random unsigned 64-bit integer using the random state. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API u64 cu_rand_get(cu_rand_state *state);

#endif

/* ==========================================================================
 *
 * --------------------------- Resources functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_RESOURCES_FUNCS

/* Memory usage and availability information. */
typedef struct cu_res_mem
{
	uptr phys_present;  /* Total physical memory on the system. */
	uptr phys_tot_used; /* Total physical memory used. */
	uptr phys_loc_used; /* Total physical memory used by this process. */

	uptr virt_present;  /* Total virtual memory on the system. */
	uptr virt_tot_used; /* Total virtual memory used. */
	uptr virt_loc_used; /* Total virtual memory used by this process. */
} cu_res_mem;

struct cu_res_cpu_cache
{
	u32 line, assoc, size;
};

/* CPU general information. */
typedef struct cu_res_cpu
{
	char name[49];
	char vendor[13];
	char padding[2];

	u32 stepping_id;
	u32 cpuid_level;
	u32 family_id;
	u32 model_id;

	u64 base_freq_hz;

	struct cu_res_cpu_cache l1d, l1i, l2, l3;

	u64 little_endian;
} cu_res_cpu;

/* Maximum size (including terminator) of string needed by byte formatting. */
#define CU_RES_BYTEFMT_MAXSIZE 10

/* Sets string to formatted digital storage text of given bytes count.
   Maximum terminated size is given by the CU_RES_BYTEFMT_MAXSIZE macro.
   A terminator is added to the end. Returns the given string. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API char *cu_res_bytefmt(char *str, u64 bytes);

/* Get memory usage information. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_res_meminfo(cu_res_mem *info);

/* Get CPU usage percentage by this current process. */
CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
CU_API real64 cu_res_cpuusage(void);

/* Get general CPU information. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_res_cpuinfo(cu_res_cpu *info);

/* Maximum size (including terminator) of string needed for OS, computer or user name. */
#define CU_RES_NAME_MAXSIZE 257

/* Get OS name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
CU_ATTRIB_NOTHROW
CU_API uptr cu_res_osname(char *namebuf);
/* Get host name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
CU_ATTRIB_NOTHROW
CU_API uptr cu_res_hostname(char *namebuf);
/* Get current user's name and place into namebuf if not NULL.
   Returns terminated length of string, or 0 on error. */
CU_ATTRIB_NOTHROW
CU_API uptr cu_res_username(char *namebuf);

#endif

/* ==========================================================================
 *
 * ----------------------------- Time functions -----------------------------
 *
 * ========================================================================== */

#if CU_SETTING_TIME_FUNCS

/* Current date & time data structure. */
typedef struct cu_tm_time
{
	int nanosec;   /* [0-1K] */
	int microsec;  /* [0-1K] */
	int millisec;  /* [0-1K] */
	int second;    /* [0-60] */
	int minute;    /* [0-59] */
	int hour;      /* [0-23] */
	int month_day; /* [1-31] */
	int month;     /* [0-11] */
	int year;      /* [year] */
	int week_day;  /* [0-6]  */
	int year_day;  /* [0-365] */
	int dst_hour;  /* -1/0/1 */

	const char *tznm; /* Abbreviated timezone name. */
	long utc_dif;     /* Seconds after UTC. */
} cu_time;

typedef i64 cu_timer;

/* Get current time. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_time_now(cu_time *tm);

/* Start the given timer. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_timer_begin(cu_timer *tm);
/* Get the microseconds passed between now and the given timer as an integer. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) CU_ATTRIB_PURE CU_ATTRIB_WARN_UNUSED_RESULT
CU_API cu_timer cu_timer_end(const cu_timer *tm);
/* Get the microseconds passed between now and the given timer as a floating-point value. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) CU_ATTRIB_PURE CU_ATTRIB_WARN_UNUSED_RESULT
CU_API real64 cu_timer_endf(const cu_timer *tm);

#endif

/* ==========================================================================
 *
 * -------------------------- Networking functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_NETWORK_FUNCS

#if CU_OS_APPLE
#  define _DARWIN_UNLIMITED_SELECT
#endif

#if !CU_OS_UNIX
#  include <winsock2.h>
   typedef SOCKET cu_socket;
#else
   typedef int cu_socket;
#endif

#define CU_NET_IPADDR_LEN 46

/* Error values returned by networking functions. */
enum cu_net_error
{
	CUERR_NONE, /* Successful call. */

	CUERR_ARGS, /* Invalid arguments. */
	CUERR_MEM, /* Could not allocate memory. */

	CUERR_GENERIC, /* Generic error. */
	CUERR_ADDR, /* Could not get address info. */
	CUERR_CONNECT, /* Could not connect to address or the remote was disconnected. */
	CUERR_LISTEN /* Server could not start listening. */
};

/* Identifiers for listen events. */
enum cu_net_event
{
	/* 'data' and 'n' of the remote event function pointer arguments are left as NULL and 0 respectively in the function pointer call unless stated otherwise. */

	/* Data has been recieved from the target remote.
	   'data' (malloc'd, needs to be free'd) and 'n' are filled with recieved data. */
	CUEVT_MESSAGE,
	/* Client connected to server.
	   On a server, 'n' is set to the updated number of connected clients. */
	CUEVT_CONNECT,
	/* Target remote disconnected.
	   On a server, 'n' is set to the updated number of connected clients and there is no need to disconnect the given client manually.
	   On a client, the listening function returns after this event and 'n' is set to whether it was a client-side disconnection. */
	CUEVT_DISCONNECT,

	/* Listen or recieve was interrupted by a signal.
	   If a recieve was interrupted, the client is given.
	   If the event handler returns 0, the listening function returns. */
	CUEVT_SIGNAL,

	/* This event can be run at a fixed interval by the server and client. */
	CUEVT_HEARTBEAT,

	/* Could not allocate some data. 'n' is set to the number of bytes that was attempted to be allocated.
	   If this occurs from a server and a client was given, this means it failed to allocate space for their sent data. */
	CUEVT_ALLOCDMEMERR,
	/* A client attempted to connect, but an error occurred while accepting the connection.
	   n=maxclients (i.e. non-zero) if this is from reaching the client limit. */
	CUEVT_REMOTECONERR,
	/* Generic issue from listening for messages. */
	CUEVT_MSGLISTENERR
};

/* Information on an open connection. */
typedef struct cu_net_remote
{
	/* Can be used to identify this remote in the context of your program. */
	void *ext;
	/* Internal value. Do not change or use for identification. */
	cu_socket fd;
	/* IP address of remote. */
	char ip[CU_NET_IPADDR_LEN + (CU_DM_64BIT * 6) + (CU_DM_32BIT * 2)];
} cu_net_remote;

struct cu_net_server;

/* Client's event handler for listening. You should return 1 unless a specific effect for the given event is desired. */
typedef int (*cu_client_event)(cu_net_remote *remote, enum cu_net_event event_type, void *data, uptr n);
/* Server's event handler for listening. You should return 1 unless a specific effect for the given event is desired. */
typedef int (*cu_server_event)(struct cu_net_server *server, cu_net_remote *remote, enum cu_net_event event_type, void *data, uptr n);

/* Server data. */
typedef struct cu_net_server
{
	/* Connected remotes. First is server's local remote, all others belong to each client. Order is not guaranteed. */
	cu_net_remote *remotes;
	/* Number of connected clients. */
	int clients_count;
	/* Maximum number of clients allowed at once as determined by user. Can be modified at any time. */
	int max_clients;
	/* Given server event handler. */
	cu_server_event event_handler;

	int remotes_capacity;
#if CU_DM_64BIT
	int _pad;
#endif
	struct pollfd *pfds;
} cu_net_server;

/* ------------ Client functions ------------ */

/* Connect to a server at the given port and address.
   Port must be in the range [1024, 65535].
   Returns CUERR_NONE on success, otherwise CUERR_ARGS, CUERR_ADDR or CUERR_CONNECT. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API enum cu_net_error cu_client_start(cu_net_remote *CU_RESTRICT server_info, const char *CU_RESTRICT address, u16 port);

/* Listens for network events related to the given server, running the event handler appropriately.
   The possible events are: CUEVT_MESSAGE, CUEVT_DISCONNECT, CUEVT_SIGNAL, CUEVT_ALLOCDMEMERR, CUEVT_MSGLISTENERR and CUEVT_HEARTBEAT.
   If the heartbeat event delay is negative, it does not occur.
   Catches SIGINT signal (set to default handler afterwards) and blocks until 0 is returned on a CUEVT_SIGNAL or the server disconnects. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API void cu_client_listen(cu_net_remote *CU_RESTRICT server_info, cu_client_event event_handler, int heartbeat_delay_msec);

/* Closes the connection to the given server. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_client_close(cu_net_remote *server_info);

/* ------------ Server functions ------------ */

/* Start a server with the given settings.
   Port must be in the range [1024, 65535] and maximum clients should be larger than 0.
   Returns CUERR_NONE on success and CUERR_ARGS, CUERR_LISTEN, CUERR_MEM or CUERR_ADDR otherwise. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API enum cu_net_error cu_server_start(cu_net_server *server, u16 port, int max_clients);

/* Listens for network events, running the server's event handler function pointer member when one occurs.
   The possible events are: CUEVT_MESSAGE, CUEVT_CONNECT, CUEVT_DISCONNECT, CUEVT_ALLOCDMEMERR, CUEVT_REMOTECONERR, CUEVT_MSGLISTENERR, CUEVT_SIGNAL and CUEVT_HEARTBEAT.
   If the heartbeat event delay is negative, it does not occur.
   Catches SIGINT signal (set to default handler afterwards) and blocks until 0 is returned on a CUEVT_SIGNAL. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_server_listen(cu_net_server *server, cu_server_event event_handler, int heartbeat_delay_msec);

/* Broadcast data to all connected clients except for the ones in the given array.
   Returns CUERR_NONE on success and CUERR_GENERIC otherwise. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API enum cu_net_error cu_server_broadcast(const cu_net_server *CU_RESTRICT server, const void *CU_RESTRICT data, uptr bytes, cu_net_remote **CU_RESTRICT except, int except_length);

/* Disconnect the given client and runs the CUEVT_DISCONNECT event. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API void cu_server_disconnect_client(cu_net_server *CU_RESTRICT server, cu_net_remote *CU_RESTRICT client);

/* Closes the server and all client connections. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API void cu_server_close(cu_net_server *server);

/* ------------ General functions ------------ */

/* Initialize networking libraries. */
CU_ATTRIB_NOTHROW
CU_API int cu_net_init(void);
/* Terminate networking libraries. */
CU_ATTRIB_NOTHROW
CU_API void cu_net_terminate(void);

/* Sends n bytes of data to the target remote.
   Returns CUERR_NONE on success and CUERR_GENERIC otherwise. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2))
CU_API enum cu_net_error cu_net_sendmsg(const cu_net_remote *CU_RESTRICT target, const void *CU_RESTRICT data, uptr n);

/* Waits until data is recieved from the target remote.
   On a memory error (CUERR_MEM), 'bytes' is set to the number of bytes it attempted to allocate.
   Returns CUERR_NONE on success and CUERR_MEM, CUERR_CONNECT (target remote sdisconnected) or CUERR_GENERIC otherwise. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2, 3))
CU_API enum cu_net_error cu_net_recvmsg(const cu_net_remote *CU_RESTRICT target, void **CU_RESTRICT data, uptr *CU_RESTRICT bytes);

/* Sets the given string to the device's local interfaces.
   At most CU_NET_IPADDR_LEN bytes of string is used, including the terminator.
   A specific interface format can be requested using the CU_NET_INTERFACE_* macros.
   You can increase 'id' to use another found interface, starting from id 0.
   Returns the given string on success or NULL on error. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API char *cu_net_interfaces(char *ipbuf, int if_fmt, int id);

/* Determine whether a given remote has been closed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_net_isclosed(const cu_net_remote *remote);

/* Returns a string describing the last encountered error. */
CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
CU_API const char *cu_net_lasterr(void);

#define CU_NET_INTERFACE_ANY 0
#define CU_NET_INTERFACE_IPV4 1
#define CU_NET_INTERFACE_IPV6 2

#endif

/* ==========================================================================
 *
 * --------------------------- Threading functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_THREAD_FUNCS

#if CU_THREAD_POSIX_AVAILABLE
#  define CU_THREAD_POSIX_USED 1
#  define CU_THREAD_WIN_USED 0
#  define CU_THREAD_C_USED 0
#  include <pthread.h>
   typedef pthread_t cu_thread;
   typedef pthread_mutex_t cu_thread_mutex;
   typedef pthread_cond_t cu_thread_conditional;
   typedef void *cu_thread_arg;
   typedef void *cu_thread_return;
   typedef cu_thread_return (*cu_thread_func)(cu_thread_arg);
#  define CU_THREAD_FUNCTION(name, argname) cu_thread_return name(cu_thread_arg argname)
#elif CU_THREAD_WIN_AVAILABLE
#  define CU_THREAD_POSIX_USED 0
#  define CU_THREAD_WIN_USED 1
#  define CU_THREAD_C_USED 0
#  include <windows.h>
   typedef HANDLE cu_thread;
   typedef HANDLE cu_thread_mutex;
   typedef LPVOID cu_thread_arg;
   typedef DWORD cu_thread_return;
   typedef cu_thread_return (WINAPI *cu_thread_func)(cu_thread_arg);
#  define CU_THREAD_FUNCTION(name, argname) DWORD WINAPI name(cu_thread_arg argname)
#elif CU_THREAD_C_AVAILABLE
#  define CU_THREAD_POSIX_USED 0
#  define CU_THREAD_WIN_USED 0
#  define CU_THREAD_C_USED 1
#  include <threads.h>
   typedef thrd_t cu_thread;
   typedef mtx_t cu_thread_mutex;
   typedef void *cu_thread_arg;
   typedef int cu_thread_return;
   typedef cu_thread_return (*cu_thread_func)(cu_thread_arg);
#  define CU_THREAD_FUNCTION(name, argname) cu_thread_return name(cu_thread_arg argname)
#endif

#define CU_THREAD_RETURN_VAL ((cu_thread_return)0)

/* Returns the number of CPUs available. */
CU_ATTRIB_NOTHROW
CU_API int cu_thread_count(void);
/* Sleeps for the given time frame. This does not affect timers. */
CU_API void cu_thread_sleep(u64 secs, u64 microsecs);

/* Get current process ID. */
CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
CU_API u32 cu_thread_pid(void);
/* Get current thread ID. */
CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
CU_API u32 cu_thread_tid(void);

/* Creates a thread. Returns 0 if failed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API cu_thread cu_thread_create(cu_thread_func function, cu_thread_arg arg);
/* Returns the current thread. */
CU_ATTRIB_NOTHROW
CU_API cu_thread cu_thread_self(void);

/* Waits for the given thread to finish. Returns 0 if failed. */
CU_ATTRIB_NOTHROW
CU_API int cu_thread_join(cu_thread thread);
/* Detaches the given thread. Returns 0 if failed. */
CU_ATTRIB_NOTHROW
CU_API int cu_thread_detach(cu_thread thread);

/* Initializes the given mutex. Returns 0 if failed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_thread_mutex_init(cu_thread_mutex *mutex);
/* Locks the given mutex. Returns 0 if failed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_thread_mutex_lock(cu_thread_mutex *mutex);
/* Unlocks the given mutex. Returns 0 if failed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_thread_mutex_unlock(cu_thread_mutex *mutex);
/* Tries to lock the given mutex. Returns 0 not acquired. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_thread_mutex_trylock(cu_thread_mutex *mutex);
/* Destroys the given mutex. Returns 0 if failed. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
CU_API int cu_thread_mutex_destroy(cu_thread_mutex *mutex);

#endif

/* ==========================================================================
 *
 * ----------------------- Debug allocation functions -----------------------
 *
 * ========================================================================== */

#if CU_SETTING_ALLOC_FUNCS

/* Custom assertion fail handler. */
CU_ATTRIB_NOTHROW CU_ATTRIB_NORETURN CU_ATTRIB_COLD
CU_API void cu_deballoc_assert_fail(int line, const char *func, const char *file, const char *cond);

CU_ATTRIB_NOTHROW
CU_API void cu_deballoc_free(int line, const char *CU_RESTRICT func, const char *CU_RESTRICT file, void *CU_RESTRICT ptr);
CU_ATTRIB_NOTHROW CU_ATTRIB_ALLOCSIZE(4) CU_ATTRIB_MALLOC_FULL(cu_deballoc_free, 4) CU_ATTRIB_WARN_UNUSED_RESULT
CU_API void *cu_deballoc_malloc(int line, const char *CU_RESTRICT func, const char *CU_RESTRICT file, uptr bytes);
CU_ATTRIB_NOTHROW CU_ATTRIB_ALLOCSIZE_MUL(4, 5) CU_ATTRIB_MALLOC_FULL(cu_deballoc_free, 4) CU_ATTRIB_WARN_UNUSED_RESULT
CU_API void *cu_deballoc_calloc(int line, const char *CU_RESTRICT func, const char *CU_RESTRICT file, uptr n, uptr size_each);
CU_ATTRIB_NOTHROW CU_ATTRIB_ALLOCSIZE(5) CU_ATTRIB_MALLOC_FULL(cu_deballoc_free, 4) CU_ATTRIB_WARN_UNUSED_RESULT
CU_API void *cu_deballoc_realloc(int line, const char *CU_RESTRICT func, const char *CU_RESTRICT file, void *CU_RESTRICT ptr, uptr bytes);

/* Allocation statistics. */
typedef struct cu_deballoc_stats
{
	/* Current memory usage in bytes. */
	uptr memusage;
	/* Highest memory usage in bytes. */
	uptr highest_memusage;

	uptr free_count; /* Number of 'free' calls. */
	uptr malloc_count; /* Number of 'malloc' calls. */
	uptr realloc_count; /* Number of 'realloc' calls. */
	uptr calloc_count; /* Number of 'calloc' calls. */

	/* Number of currently allocated pointers. */
	uptr allocd_ptrs;
	/* Number of times an allocation function returned NULL. */
	uptr alloc_fails;

	/* Number of times an out-of-bounds write was detected. */
	uptr underruns, overruns;
	/* Number of times an invalid call to an allocation function was made. */
	uptr invalid_free, invalid_realloc, empty_realloc;
} cu_deballoc_stats;

/* Returns current allocation statistics.
   Do not save the stored pointer, as this also checks for buffer under/overruns. */
CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT CU_ATTRIB_RETURNS_NONNULL CU_ATTRIB_COLD
CU_API const cu_deballoc_stats *cu_deballoc_getstats(void);
/* Prints out summary of allocation statistics and all detected allocation errors. */
CU_ATTRIB_NOTHROW CU_ATTRIB_COLD
CU_API void cu_deballoc_summary(void);

/* Initializes debug allocation. Verbose logs also include allocations and deallocations. */
CU_ATTRIB_NOTHROW CU_ATTRIB_COLD
CU_API int cu_deballoc_start(int log_file, int log_file_verbose);
/* Cleans up resources used for debug allocation functions. */
CU_ATTRIB_NOTHROW CU_ATTRIB_COLD
CU_API void cu_deballoc_end(void);

#include <stdlib.h>

#define CU_FREE(ptr) do { free(ptr); ptr = NULL; } while (0)

#if CU_DEBUG && CU_SETTING_CUSTOM_ALLOCS
#  define CU_REPLACED_ALLOC 1
#  define malloc(n) cu_deballoc_malloc(CU_LINE, CU_FUNC, CU_FILE, n)
#  define realloc(p, n) cu_deballoc_realloc(CU_LINE, CU_FUNC, CU_FILE, p, n)
#  define calloc(n, s) cu_deballoc_calloc(CU_LINE, CU_FUNC, CU_FILE, n, s)
#  define free(p) cu_deballoc_free(CU_LINE, CU_FUNC, CU_FILE, p)
#else
#  define CU_REPLACED_ALLOC 0
#endif

#endif /* CU_SETTING_ALLOC_FUNCS */

#endif /* CU_SETTING_FUNCS */

#if defined (__cplusplus)
}
#endif

#endif /* CUTILS_HDR */
