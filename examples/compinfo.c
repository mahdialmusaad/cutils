#include "../cutils.h"

#include <stdlib.h>
#include <stdio.h>

static const struct { const char *flag, *desc; } options[] = {
	{ "a", "all computer information" },
	{ "p", "present amount of memory" },
	{ "m", "available amount of memory" },
	{ "n", "CPU name and vendor" },
	{ "c", "CPU cache and core information" },
	{ "f", "CPU ID values" },
	{ "s", "CPU clock speed" },
	{ "h", "hostname" },
	{ "u", "current username" },
	{ "o", "OS name" },
	{ "i", "network interfaces" }
};
#define NOPTS ((int)(sizeof options / sizeof *options))

static const char *arch_names[] = { "Unknown", "x86", "ARM" };
static int active[NOPTS];

int main(int argc, char **argv)
{
	char nmbuf[CU_RES_NAME_MAXSIZE], ipbuf[CU_NET_IPADDR_LEN], *ipres = NULL, *carg, *start;
	int i, j, valid, err;
	cu_res_cpu cpuinfo;
	cu_res_mem meminfo;

	if (argc == 1) {
	fail:
		fprintf(stderr, "Usage: %s [OPTIONS]\nOptions:\n", argv[0]);
		for (i = 0; i < NOPTS; ++i) fprintf(stderr, "  -%c  Show %s.\n", (int)*options[i].flag, options[i].desc);
		fprintf(stderr, "Send all bug reports and suggestions to https://github.com/mahdialmusaad/cutils.\n");
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
			valid = 0;
			for (j = 0; j < NOPTS; ++j) {
				if (*options[j].flag != *carg && *carg != 'a') continue;
				active[j] = valid = 1;
			}
			if (!valid) goto invalid;
		}
	}

	err = 0;

	if ((active[1] || active[2]) && !cu_res_meminfo(&meminfo)) {
		fprintf(stderr, "Failed to retrieve memory info.\n");
		err = 1;
	} else if (active[1] || active[2]) {
		if (active[1]) printf("%" CU_U64_FMT "B phys + %" CU_U64_FMT "B swap\n", meminfo.physical_present, meminfo.virtual_present - meminfo.physical_present);
		if (active[2]) printf("%" CU_U64_FMT "B phys free + %" CU_U64_FMT "B swap free\n", meminfo.physical_free, meminfo.virtual_free - meminfo.physical_free);
	}

	if (active[3] || active[4] || active[5] || active[6]) {
		cu_res_cpuinfo(&cpuinfo, active[3] || active[5], active[4], active[6]);
		if (active[3]) printf("Name: %s\nVendor: %s\nArchitecture: %s\n", cpuinfo.name, cpuinfo.vendor, arch_names[cpuinfo.arch]);
		if (active[4]) {
			printf(
				"L1i: %uB (assoc=%d, %uB line)\nL1d: %uB (assoc=%d, %uB line)\nL2: %uB (assoc=%d, %uB line)\nL3: %uB (assoc=%d, %uB line)\nLogical cores: %u\nProcessors: %u\n",
				cpuinfo.l1i.size, cpuinfo.l1i.assoc, cpuinfo.l1i.line,
				cpuinfo.l1d.size, cpuinfo.l1d.assoc, cpuinfo.l1d.line,
				cpuinfo.l2.size, cpuinfo.l2.assoc, cpuinfo.l2.line,
				cpuinfo.l3.size, cpuinfo.l3.assoc, cpuinfo.l3.line,
				cpuinfo.logical_processors, cpuinfo.processor_cores
			);
		}
		if (active[5]) printf("Family: %u\nModel: %u\nStepping: %u\n", cpuinfo.family_id, cpuinfo.model_id, cpuinfo.stepping_id);
		if (active[6]) {
			printf(
				"Max: %" CU_U64_FMT "MHz, Min: %" CU_U64_FMT "MHz, Base: %" CU_U64_FMT "MHz, Now: %" CU_U64_FMT "MHz\n",
				cpuinfo.max_freq_hz / 1000 / 1000, cpuinfo.min_freq_hz / 1000 / 1000, cpuinfo.base_freq_hz / 1000 / 1000, cpuinfo.cur_freq_hz / 1000 / 1000
			);
		}
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
