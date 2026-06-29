#include "tests.h"

#include "cu_macro1.c"
#include "custr1.c"
#include "custr2.c"
#include "custr3.c"
#include "custr4.c"
#include "cu_cont1.c"
#include "cu_file1.c"
#include "cu_res1.c"
#include "cu_time1.c"
#include "cu_net1.c"
#include "cu_net2.c"
#include "cu_net3.c"
#include "cu_thread1.c"
#include "cu_thread2.c"

static void cu_test_given(void (*base)(void), const char *basename)
{
	memset(buf, '\255', sizeof buf);
	tested = failed = fline = 0;
	f_expr_got = f_expr_exp = 0;
	f_expr = NULL;
	if (e_argc == 1) printf("%s.c \t", basename);
	fflush(stdout);
	base();
	if (e_argc == 1) printf("[%d/%d]", tested-failed, tested);
	if (fline) printf(" L%d: \"%s\": got %" CU_UPTR_FMT ", expected %d\n", fline, f_expr, f_expr_got, f_expr_exp);
	else if (e_argc == 1) printf("\n");
}

#define TEST(base) cu_test_given(base, CU_STRINGIFY(base))

int main(int argc, char **argv)
{
	e_argc = argc;
	e_argv = argv;

	TEST(cu_macro1);
	TEST(custr1);
	TEST(custr2);
	TEST(custr3);
	TEST(custr4);
	TEST(cu_cont1);
	TEST(cu_file1);
	TEST(cu_res1);
	TEST(cu_time1);
	TEST(cu_thread1);
	TEST(cu_thread2);
	TEST(cu_net1);
	TEST(cu_net2);
	TEST(cu_net3);

	if (e_argc != 1 && !gfailed) printf("All tests successful.\n");
	return gfailed != 0;
}
