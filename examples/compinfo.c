#include "../cutils.h"

#include <stdlib.h>
#include <stdio.h>

struct
{
	char flag;
	const char *desc;
} const options[] = {
	{ 'a', "all computer information" },
	{ 'p', "present amount of memory" },
	{ 'm', "available amount of memory" },
	{ 'n', "CPU name and vendor" },
	{ 'c', "CPU cache information" },
	{ 'f', "CPU ID values" },
	{ 's', "CPU clock speed" },
	{ 'h', "hostname" },
	{ 'u', "current username" },
	{ 'o', "OS name" },
	{ 'i', "network interfaces" }
};

#define NOPTS ((int)(sizeof options / sizeof *options))
int active[NOPTS];

int main(int argc, char **argv)
{
	char nmbuf[CU_RES_NAME_MAXSIZE], ipbuf[CU_NET_IPADDR_LEN], *ipres, *carg, *start;
	int i, j, valid, err;
	cu_res_cpu cpuinfo;
	cu_res_mem meminfo;

	if (argc == 1) {
	fail:
		fprintf(stderr, "Usage: %s [OPTIONS]\nOptions:\n", argv[0]);
		for (i = 0; i < NOPTS; ++i) fprintf(stderr, "  -%c  Show %s.\n", (int)options[i].flag, options[i].desc);
		fprintf(stderr, "If you think any given information is incorrect or it always fails to retrieve it, please submit a bug report at https://github.com/mahdialmusaad/cutils.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i < argc; ++i) {
		start = carg = argv[i];
		if (*carg == '\0' || *carg != '-' || carg[1] == '\0') {
		invalid:
			fprintf(stderr, "Invalid option '%s'.\n", start);
			goto fail;
		}

		while (*(++carg)) {
			for (j = 0; j < NOPTS; ++j) {
				if (options[j].flag != *carg && *carg != 'a') continue;
				active[j] = valid = 1;
			}

			if (!valid) goto invalid;
			valid = 0;
		}
	}

	err = 0;

	if ((active[1] || active[2]) && !cu_res_meminfo(&meminfo)) {
		fprintf(stderr, "Failed to retrieve memory info.\n");
		err = 1;
	} else if (active[1] || active[2]) {
		if (active[1]) printf("%" CU_U64_FMT "B + %" CU_U64_FMT "B swap\n", meminfo.physical_present, meminfo.virtual_present - meminfo.physical_present);
		if (active[2]) printf("%" CU_U64_FMT "B free\n", meminfo.physical_free);
	}

	if ((active[3] || active[4] || active[5] || active[6]) && !cu_res_cpuinfo(&cpuinfo)) {
		fprintf(stderr, "Failed to retrieve CPU info.\n");
		err = 1;
	} else if (active[3] || active[4] || active[5] || active[6]) {
		if (active[3]) printf("Name: %s\nVendor: %s\n", cpuinfo.name, cpuinfo.vendor);
		if (active[4]) {
			printf(
				"L1i: %uB (%uB assoc, %uB line)\nL1d: %uB (%uB assoc, %uB line)\nL2: %uB (%u assoc, %uB line)\nL3: %uB (%u assoc, %uB line)\n",
				cpuinfo.l1i.size, cpuinfo.l1i.assoc, cpuinfo.l1i.line,
				cpuinfo.l1d.size, cpuinfo.l1d.assoc, cpuinfo.l1d.line,
				cpuinfo.l2.size, cpuinfo.l2.assoc, cpuinfo.l2.line,
				cpuinfo.l3.size, cpuinfo.l3.assoc, cpuinfo.l3.line);
		}
		if (active[5]) printf("Family: %d\nModel: %d\nStepping: %d\n", cpuinfo.family_id, cpuinfo.model_id, cpuinfo.stepping_id);
		if (active[6]) printf("Base: %" CU_U64_FMT "Hz %s\n", cpuinfo.base_freq_hz, cpuinfo.cpuid_level < 0x16 ? "(Measured/Parsed)" : "");
	}

	if (active[7] && !cu_res_hostname(nmbuf)) {
		fprintf(stderr, "Failed to retrieve hostname.\n");
		err = 1;
	} else if (active[7]) printf("Hostname: %s\n", nmbuf);

	if (active[8] && !cu_res_username(nmbuf)) {
		fprintf(stderr, "Failed to retrieve username.\n");
		err = 1;
	} else if (active[8]) printf("Username: %s\n", nmbuf);

	if (active[9] && !cu_res_osname(nmbuf)) {
		fprintf(stderr, "Failed to retrieve OS name.\n");
		err = 1;
	} else if (active[9]) printf("OS name: %s\n", nmbuf);

	if (active[10] && !(ipres = cu_net_interfaces(ipbuf, CU_NET_INTERFACE_ANY, 0))) {
		fprintf(stderr, "Failed to retrieve any network interface.");
		err = 1;
	} else if (active[10]) {
		printf("Interfaces:\n");
		for (i = 1; ipres; ++i) {
			printf("%d: %s\n", i, ipbuf);
			ipres = cu_net_interfaces(ipbuf, CU_NET_INTERFACE_ANY, i);
		}
	}

	return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
