#include "tests.h"

#if CU_SETTING_RESOURCES_FUNCS

TFUNC(cu_res1)
{
	cu_res_cpu cpu;
	cu_res_mem mem;

	EXPECT(cu_res_meminfo(&mem));
	cu_res_cpuinfo(&cpu, 1, 1, 1);
	EXPECT(*cpu.name);
	EXPECT(cpu.max_freq_hz || cpu.min_freq_hz);
	EXPECT(cpu.logical_cores >= cpu.phyiscal_cores && cpu.phyiscal_cores > 0);
	EXPECT(cpu.l1d.size && cpu.l1d.line && cpu.l1d.assoc);
	EXPECT(cu_res_crypto(buf, sizeof buf) == sizeof buf);
	EXPECT(cu_res_osname(buf));
	EXPECT(cu_res_hostname(buf));
	EXPECT(cu_res_username(buf));
	EXPECT0(cu_res_cpuusage());
	EXPECT0(((cu_res_bytefmt(buf, 0)), (strcmp(buf, "0B"))));
	EXPECT0(((cu_res_bytefmt(buf, 2)), (strcmp(buf, "2B"))));
	EXPECT0(((cu_res_bytefmt(buf, 21)), (strcmp(buf, "21B"))));
	EXPECT0(((cu_res_bytefmt(buf, 214)), (strcmp(buf, "214B"))));
	EXPECT0(((cu_res_bytefmt(buf, 2147)), (strcmp(buf, "2.1KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 21474)), (strcmp(buf, "21.5KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 214748)), (strcmp(buf, "214.7KB"))));
	EXPECT0(((cu_res_bytefmt(buf, 2147483)), (strcmp(buf, "2.1MB"))));
	EXPECT0(((cu_res_bytefmt(buf, 21474836)), (strcmp(buf, "21.5MB"))));
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
