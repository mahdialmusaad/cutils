#include "tests.h"

#if !CU_OS_WINDOWS
#  define GR "\033[0;32m"
#  define RD "\033[1;31m"
#  define RS "\033[0m"
#else
#  define RD ""
#  define GR ""
#  define RS ""
#endif

static void cu_test_given(void (*base)(void), const char *basename)
{
	memset(buf, '\255', sizeof buf);
	tested = failed = fline = 0;
	f_expr_got = f_expr_exp = 0;
	f_expr = NULL;
	printf("%s.c \t", basename);
	fflush(stdout);
	base();
	printf("%s[%d/%d]%s", !failed ? GR : RD, tested-failed, tested, RS);
	if (fline) printf(" L%d: \"%s\": got %" CU_UPTR_FMT ", expected %d\n", fline, f_expr, f_expr_got, f_expr_exp);
	else printf("\n");
}

#define TEST(base) cu_test_given(base, CU_STRINGIFY(base))

#include "cu_thread1.c"
#include "cu_macro1.c"
#include "cu_file1.c"
#include "cu_res1.c"
#include "cu_net1.c"
#include "custr1.c"
#include "custr2.c"
#include "custr3.c"
#include "custr4.c"

int main(int argc, char **argv)
{
	e_argc = argc;
	e_argv = argv;

	TEST(cu_macro1);
	TEST(custr1);
	TEST(custr2);
	TEST(custr3);
	TEST(custr4);
	TEST(cu_file1);
	TEST(cu_res1);
	TEST(cu_thread1);
	TEST(cu_net1);
	return 0;
}
