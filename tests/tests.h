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
static int f_expr_exp;

static int tested;
static int failed, gfailed;
static int fline;

static char buf[1024];

static void cu_test_expect(const char *expr, int line, uptr res, int want)
{
	++tested;
	if (!!(res) == want) return;
	++failed;
	++gfailed;
	if (f_expr) return;
	f_expr = expr;
	f_expr_got = res;
	f_expr_exp = want;
	fline = line;
}

#define EXPECT(expr)  cu_test_expect(CU_STRINGIFY(expr), CU_LINE, (uptr)(expr), 1)
#define EXPECT0(expr) cu_test_expect(CU_STRINGIFY(expr), CU_LINE, (uptr)(expr), 0)

#define TFUNC(name) CU_ATTRIB_NOTHROW CU_ATTRIB_USED static void name(void); void name(void)

#endif
