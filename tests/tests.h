#ifndef CUTILS_TEST
#define CUTILS_TEST

#include "../cutils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

static char **e_argv;
static CU_ATTRIB_USED int e_argc;

static const char *f_expr;
static uptr f_expr_got;
static uptr f_expr_exp;

static int tested;
static int failed;
static int fline;

static char buf[1024];

#define FAILTEST(expr, got, exp) do { ++failed; if (!f_expr) { f_expr = CU_STRINGIFY(expr); f_expr_got = (uptr)(got); f_expr_exp = (uptr)(exp); fline = CU_LINE; } } while (0)
#define EXPECT(expr)  do { uptr _e = (uptr)(expr); ++tested; if (_e); else FAILTEST(expr, _e, 1); } while (0)
#define EXPECT0(expr) do { uptr _e = (uptr)(expr); ++tested; if (_e)       FAILTEST(expr, _e, 0); } while (0)

#define TFUNC(name) CU_ATTRIB_NOTHROW CU_ATTRIB_USED static void name(void); void name(void)

#endif
