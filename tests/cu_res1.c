#include "tests.h"

#if CU_SETTING_RESOURCES_FUNCS

TFUNC(cu_res1)
{
	cu_res_cpu cpu;
	cu_res_mem mem;
	volatile int i;

	EXPECT(cu_res_meminfo(&mem));
	EXPECT(cu_res_cpuinfo(&cpu));
	EXPECT(cu_res_crypto(buf, sizeof buf) == sizeof buf);
	EXPECT(cu_res_osname(buf));
	EXPECT(cu_res_hostname(buf));
	EXPECT(cu_res_username(buf));
	EXPECT0(cu_res_cpuusage());
	for (i = 0; i < 100000000; ++i);
	EXPECT(cu_res_cpuusage() > 0.0);
	EXPECT0(((cu_res_bytefmt(buf, 0)), (strcmp(buf, "0B"))));
	EXPECT0(((cu_res_bytefmt(buf, 2)), (strcmp(buf, "2B"))));
	EXPECT0(((cu_res_bytefmt(buf, 21)), (strcmp(buf, "21B"))));
	EXPECT0(((cu_res_bytefmt(buf, 214)), (strcmp(buf, "214B"))));
	EXPECT0(((cu_res_bytefmt(buf, 2147)), (strcmp(buf, "2.1KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 21474)), (strcmp(buf, "21.4KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 214748)), (strcmp(buf, "214.7KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 2147483)), (strcmp(buf, "2.1MB"))));
	EXPECT0(((cu_res_bytefmt(buf, 21474836)), (strcmp(buf, "21.4MB"))));
	EXPECT0(((cu_res_bytefmt(buf, 214748364)), (strcmp(buf, "214.7MB"))));
	EXPECT0(((cu_res_bytefmt(buf, 2147483647)), (strcmp(buf, "2.1GB"))));
	EXPECT0(((cu_res_bytefmt(buf, 1000)), (strcmp(buf, "1.0KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 1001)), (strcmp(buf, "1.0KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 1010)), (strcmp(buf, "1.0KB"))));
	EXPECT0(((cu_res_bytefmt(buf, CU_U64_C(1234567891011))), (strcmp(buf, "1.2TB"))));
	EXPECT0(((cu_res_bytefmt(buf, CU_U64_C(1234567891011121))), (strcmp(buf, "1.2PB"))));
	EXPECT0(((cu_res_bytefmt(buf, CU_U64_C(1234567891011121314))), (strcmp(buf, "1.2EB"))));
}

#else
TFUNC(cu_res1) { }
#endif
