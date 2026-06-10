/*
   Tests for provided functions from the cutils library.
   Main library code and documentation can found in cutils.c and cutils.h in the same directory.
   The cutils library is licensed under the GPLv2, see aforementioned files for full statement.
*/

#include "cutils.h"
#include <string.h>
#include <stdio.h>

#if CU_OS_WINDOWS
#  define PATH_PREFIX "C:"
#  define COL_RESET ""
#  define COL_BOLD ""
#  define COL_BLUE ""
#  define COL_RED ""
#  define COL_GREEN ""
#else
#  define PATH_PREFIX ""
#  define COL_RESET "\033[0m"
#  define COL_BOLD "\033[1;37m"
#  define COL_BLUE "\033[0;34m"
#  define COL_RED "\033[1;31m"
#  define COL_GREEN "\033[0;32m"
#endif

#define SEP CU_FILE_SEPARATOR_DQ
#define SEP2 SEP SEP

static int results[3];

static const char *cu_cond(int result)
{
	results[2] = !!result;
	++results[results[2]];
	return results[2] ? COL_GREEN "Pass" COL_RESET : COL_RED "FAIL" COL_RESET;
}

#if CU_COMP_GNU
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
CU_PRAGMA(GCC diagnostic ignored "-Wpedantic")
#endif

#define VA_C ,
#define N 500

static int thread_counter;
static cu_thread_mutex mutex;
static u32 tid;

static CU_THREAD_FUNCTION(cu_test_add, t_arg)
{
	int i;
	for (i = 0; i < N; ++i) {
		cu_thread_mutex_lock(&mutex);
		++thread_counter;
		cu_thread_mutex_unlock(&mutex);
	}
	if (!t_arg) tid = cu_thread_tid();
	return CU_THREAD_RETURN_VAL;
}

static FILE *printfile;

int main(int argc, char **argv)
{
	cu_res_cpu cpu;
	cu_timer start, end;
	cu_ctime now;
	cu_res_mem mem;
	cu_thread thr;
	u64 rnd = 0;
	custr c;
	uptr sr;
	int i, res;
	void *read_res;

	char *exe;
	char hwnamebuf[CU_RES_NAME_MAXSIZE];
	const char original[] = PATH_PREFIX SEP2 SEP "abc" SEP "123" SEP2 SEP ".." SEP "." SEP "456" SEP "test1" SEP2 ".." SEP "test1" SEP2 "." SEP "def" SEP2 ".." SEP "789" SEP2 SEP2 "." SEP;
	const char *parents[] = { PATH_PREFIX SEP, PATH_PREFIX SEP, PATH_PREFIX SEP "abc" SEP, PATH_PREFIX SEP "abc" SEP "456" SEP, PATH_PREFIX SEP "abc" SEP "456" SEP "test1" SEP };
	const char test_text[] = "Test text for test text file. This is 68 bytes excluding terminator.";
	const char search_replace_text[] = "Example string for S+R tests. More Example string.";
	const char opposite_search_text[] = "////////.//////.///";
	char batch_name[6] = { 'f', 'i', 'l', 'e', '#', '\0' };
	char test_text_tmp[sizeof test_text];

	char ipaddrres[CU_NET_IPADDR_LEN];
	char bytefmtres[CU_RES_BYTEFMT_MAXSIZE];

#if 0
	if (fopen_s(&printfile, "stdout.txt", "w")) return 0;
#else
	printfile = stdout;
#endif

	CU_UNUSED(argc);
	cu_timer_fill(&start);
	cu_time_now(&now);
	
	fprintf(printfile, COL_BOLD "System information:\n" COL_RESET);
	fprintf(printfile, COL_BLUE "TIME:" COL_RESET " %d:%02d:%02d.%03d %02d/%02d/%d (DST=%d)\n", now.hour, now.minute, now.second, now.millisec, now.month_day, now.month, now.year, now.isdst);
	fprintf(printfile, COL_BLUE "TIMEZONE:" COL_RESET " %s (UTC + %ds)\n", now.tznm, now.utcdif);
	sr = cu_res_osname(hwnamebuf);
	fprintf(printfile, COL_BLUE "OS:" COL_RESET " %s (%d/%d chars) (%s)\n", hwnamebuf, (int)sr, (int)(strlen(hwnamebuf)) + 1, cu_cond(sr == ((uptr)strlen(hwnamebuf) + 1)));
	sr = cu_res_username(hwnamebuf);
	fprintf(printfile, COL_BLUE "USER:" COL_RESET " %s (%d/%d chars) (%s)\n", hwnamebuf, (int)sr, (int)(strlen(hwnamebuf)) + 1, cu_cond(sr == ((uptr)strlen(hwnamebuf) + 1)));
	sr = cu_res_hostname(hwnamebuf);
	fprintf(printfile, COL_BLUE "HOST:" COL_RESET " %s (%d/%d chars) (%s)\n", hwnamebuf, (int)sr, (int)(strlen(hwnamebuf)) + 1, cu_cond(sr == ((uptr)strlen(hwnamebuf) + 1)));
	fprintf(printfile, COL_BLUE "NETWORK INTERFACES:" COL_RESET);

	for (i = 0; ; ++i) {
		char *interfaceres = cu_net_interfaces(ipaddrres, CU_NET_INTERFACE_ANY, i);
		if (!interfaceres) break;
		fprintf(printfile, "\n  %d: %s", i + 1, ipaddrres);
	}
	fprintf(printfile, "\n  Found %d interfaces (%s)\n", i, cu_cond(i != 0));

	cu_res_cpuinfo(&cpu);
	fprintf(printfile, COL_BLUE "CPU:\n" COL_RESET);
	fprintf(printfile, COL_BLUE "  Name:" COL_RESET " %s\n", cpu.name);
	fprintf(printfile, COL_BLUE "  Vendor:" COL_RESET " %s\n", cpu.vendor);
	fprintf(printfile, COL_BLUE "  Clock speed:" COL_RESET " %uMHz\n", (unsigned)(cpu.base_freq_hz / 1000000));
	fprintf(printfile, COL_BLUE "  Endianness:" COL_RESET " %s\n", cpu.little_endian ? "Little" : "Big");
	fprintf(printfile, COL_BLUE "  Stepping:" COL_RESET " %u\n", cpu.stepping_id);
	fprintf(printfile, COL_BLUE "  Model:" COL_RESET " %u\n", cpu.model_id);
	fprintf(printfile, COL_BLUE "  Family:" COL_RESET " %u\n", cpu.family_id);
	fprintf(printfile, COL_BLUE "  Threads:" COL_RESET " %d\n", cu_thread_count());
	fprintf(printfile, COL_BLUE "  Caches:" COL_RESET "\n");
	fprintf(printfile, COL_BLUE "    L1i:" COL_RESET " Line: %uB; Associativity: %u; Size: %uB (%s)\n", cpu.l1d.line, cpu.l1d.assoc, cpu.l1d.size, cu_res_bytefmt(bytefmtres, cpu.l1d.size));
	fprintf(printfile, COL_BLUE "    L1d:" COL_RESET " Line: %uB; Associativity: %u; Size: %uB (%s)\n", cpu.l1i.line, cpu.l1i.assoc, cpu.l1i.size, cu_res_bytefmt(bytefmtres, cpu.l1i.size));
	fprintf(printfile, COL_BLUE "    L2:" COL_RESET "  Line: %uB; Associativity: %u; Size: %uB (%s)\n", cpu.l2.line, cpu.l2.assoc, cpu.l2.size, cu_res_bytefmt(bytefmtres, cpu.l2.size));
	fprintf(printfile, COL_BLUE "    L3:" COL_RESET "  Line: %uB; Associativity: %u; Size: %uB (%s)\n", cpu.l3.line, cpu.l3.assoc, cpu.l3.size, cu_res_bytefmt(bytefmtres, cpu.l3.size));

	if (cu_res_meminfo(&mem)) {
		fprintf(printfile, COL_BOLD "\nFound memory info:\n" COL_RESET);
		fprintf(printfile, COL_BLUE "Total virtual:" COL_RESET "\t%" CU_UPTR_FMT "B\n", mem.virt_present);
		fprintf(printfile, COL_BLUE "Total physical:" COL_RESET "\t%" CU_UPTR_FMT "B\n", mem.phys_present);
		fprintf(printfile, COL_BLUE "Virtual used:" COL_RESET "\t%" CU_UPTR_FMT "B\n", mem.virt_tot_used);
		fprintf(printfile, COL_BLUE "Physical used:" COL_RESET "\t%" CU_UPTR_FMT "B\n", mem.phys_tot_used);
		(void)(cu_cond(1));
	} else {
		fprintf(printfile, COL_RED "\nUnable to retrieve memory info.\n" COL_RESET);
		(void)(cu_cond(0));
	}


	fprintf(printfile, COL_BOLD "\nByte formatting test:\n" COL_RESET);

	#define BYTES_TEST(size, expected) \
	cu_res_bytefmt(bytefmtres, size); \
	fprintf(printfile,  \
		COL_BLUE "FMT TEST:" COL_RESET " Bytes: %" CU_U64_FMT ", Expected fmt string: '%s', got: '%s' (%s). Expected %" CU_UPTR_FMT " chars, got %" CU_UPTR_FMT " chars.\n", \
		CU_U64_C(size), expected, bytefmtres, cu_cond(strcmp(bytefmtres, expected) == 0), (uptr)strlen(expected), (uptr)strlen(bytefmtres) \
	)

	BYTES_TEST(1234, "1.234KB");
	BYTES_TEST(CU_U64MAX, "18.446EB");
	BYTES_TEST(12345, "12.345KB");
	BYTES_TEST(123456, "123.456KB");
	BYTES_TEST(1234567, "1.234MB");
	BYTES_TEST(1000000000, "1.000GB");


	custr_create(&c, "TestTestTest");
	fprintf(printfile, COL_BOLD "\nString tests:" COL_RESET "\nCreating insert example string \"%s\"\n", c.str);
	fprintf(printfile, COL_BLUE "STRING CREATION TEST:" COL_RESET " Expected capacity of 16 and correct string (%s)\n", cu_cond(c.len == strlen(c.str) && c.cap == 16));

	#define INSERT_TEST(ins, off, expected) \
	if (!custr_insert(&c, off, custr_c(ins), 0)) fprintf(printfile, "Inserting '%s' to string (offset %d). Expected result: '%s', returned 0 instead.\n", ins, off, expected); \
	fprintf(printfile,  \
		COL_BLUE "INSERT TEST:" COL_RESET " Inserting '%s' to string (offset %d). Expected result: '%s', got '%s' (%s). Expected %" CU_UPTR_FMT " chars, got %" CU_UPTR_FMT " chars, stored %" CU_UPTR_FMT " len (%s).\n", \
		ins, off, expected, c.str, cu_cond(strcmp(c.str, expected) == 0), (uptr)strlen(expected), (uptr)strlen(c.str), c.len, cu_cond(c.len == strlen(c.str) && c.len && c.len == strlen(expected)) \
	)

	INSERT_TEST("inserted0", 0, "inserted0TestTestTest");
	INSERT_TEST("inserted1", 13, "inserted0Testinserted1TestTest");
	INSERT_TEST("inserted2", 26, "inserted0Testinserted1Testinserted2Test");
	INSERT_TEST("inserted3", 39, "inserted0Testinserted1Testinserted2Testinserted3");

	res = custr_count(&c, 'e');
	fprintf(printfile, COL_BLUE "COUNT TEST:" COL_RESET " Expected 11 instances of 'e' char, got %d (%s)\n", res, cu_cond(res == 11));
	res = custr_countsub(&c, custr_c("Test"));
	fprintf(printfile, COL_BLUE "COUNT TEST:" COL_RESET " Expected 3 instances of 'Test' substring, got %d (%s)\n", res, cu_cond(res == 3));

	custr_clear(&c);

	for (i = 0; i < 26; ++i) {
		custr *t = custr_char((char)(97 + i));
		custr_append(&c, t, 0);
	}

	fprintf(printfile,
		COL_BLUE "APPEND TEST:" COL_RESET " %s (%s), expected 26 chars and len, got %" CU_UPTR_FMT " chars and %" CU_UPTR_FMT " stored len. (%s)\n",
		c.str, cu_cond(strcmp(c.str, "abcdefghijklmnopqrstuvwxyz") == 0), (uptr)strlen(c.str), c.len, cu_cond(c.len == strlen(c.str) && c.len == 26)
	);
	custr_clear(&c);

	custr_create(&c, original);
	custr_simplify(&c);

	fprintf(printfile, COL_BLUE "Original:" COL_RESET " %s\n", original);
	fprintf(printfile, COL_BLUE "Simplified:" COL_RESET " %s (%s)\n", c.str, cu_cond(strcmp(c.str, PATH_PREFIX SEP "abc" SEP "456" SEP "test1" SEP "789" SEP) == 0));
	if (!res) {
		fprintf(printfile, COL_RED "Skipping parent test.\n");
		cu_cond(0);
		goto skip_parents;
	}

	fprintf(printfile, COL_BLUE "Parents: " COL_RESET);
	for (i = 4; i >= 0; --i) {
		custr_cd(&c, NULL);
		fprintf(printfile, "%s (%s)%s", c.str, cu_cond(strcmp(c.str, parents[i]) == 0), i ? ", " : "");
		fflush(stdout);
	}

skip_parents:
	fprintf(printfile, "\nUsing search+replace example string \"%s\" and opposite search example string \"%s\"\n", search_replace_text, opposite_search_text);

	#define SEARCH_TEST(func, offset, target, nth, expected) \
	fprintf(printfile, COL_BLUE "SEARCH TEST:" COL_RESET " Function: '%s', offset: %d, target: %s (char no. %d), expected index: %d,", #func, offset, #target, nth, expected); \
	fflush(stdout); \
	sr = func(&c, offset, target, nth); \
	fprintf(printfile, " got %" CU_UPTR_FMT " (%s)\n", sr, cu_cond(sr == expected))

	custr_create(custr_clear(&c), opposite_search_text);
	SEARCH_TEST(custr_findnot, 0, '/', 0, 8);
	SEARCH_TEST(custr_findnot, 4, '/', -1, 8);

	custr_create(custr_clear(&c), search_replace_text);

	SEARCH_TEST(custr_find, 0, 'e', 1, 24);
	SEARCH_TEST(custr_find, 9, 'e', -1, 33);
	SEARCH_TEST(custr_findsub, 0, custr_c("le"), 1, 40);
	SEARCH_TEST(custr_findsub, 0, custr_c("le "), -1, 40);

	#define REPLACE_TEST(func, offset, target, replacement, expected) \
	fprintf(printfile, COL_BLUE "REPLACE TEST:" COL_RESET " %s with %s in test string, offset %d. ", #target, #replacement, offset); \
	func(&c, offset, target, replacement); \
	fprintf(printfile, "Result: \"%s\" (%s)\n", c.str, cu_cond(strcmp(c.str, expected) == 0))

	REPLACE_TEST(custr_replace, 0, '+', '&', "Example string for S&R tests. More Example string.");
	REPLACE_TEST(custr_replace, 0, '&', '\0', "Example string for SR tests. More Example string.");
	REPLACE_TEST(custr_replace, 0, '.', ';', "Example string for SR tests; More Example string;");
	REPLACE_TEST(custr_replace, 17, 'o', 'e', "Example string for SR tests; Mere Example string;");

	REPLACE_TEST(custr_replacesub, 1, custr_c("Example"), custr_c("test"), "Example string for SR tests; Mere test string;");
	REPLACE_TEST(custr_replacesub, 0, custr_c("string"), custr_c("text"), "Example text for SR tests; Mere test text;");
	REPLACE_TEST(custr_replacesub, 0, custr_c("text"), custr_c("string"), "Example string for SR tests; Mere test string;");

	custr_clear(&c);

	#define FMT_TEST(fmt, va, expected) \
	fprintf(printfile, COL_BLUE "FMT TEST:" COL_RESET " '%s' with args " #va ", expected '%s' of len %d", fmt, expected, (int)strlen(expected)); \
	if (!custr_fmt(&c, fmt, va)) fprintf(printfile, ", returned 0 instead (%s)\n", cu_cond(0)); \
	else fprintf(printfile, ", got '%s' of len %d/%d (%s)\n", c.str, (int)c.len, (int)strlen(c.str), cu_cond(strcmp(expected, c.str) == 0)); \
	custr_clear(&c)

	FMT_TEST("%d %s %.2f", 123 VA_C "fmt_str" VA_C 1.23, "123 fmt_str 1.23");
	FMT_TEST("%%, %c, %X", 'c' VA_C 255u, "%, c, FF");


	fprintf(printfile, COL_BOLD "\nFile management tests:\n" COL_RESET);
	exe = cu_file_exe_path(*argv, NULL);
	fprintf(printfile, COL_BLUE "EXE PATH:" COL_RESET " %s (%s)\n", exe, cu_cond(exe != NULL));
	if (!exe) goto fail_exe;
	res = cu_file_write("test.txt", test_text, CU_FILE_WRITETXT, strlen(test_text));
	fprintf(printfile, COL_BLUE "FILE CREATION TEST:" COL_RESET " Writing test text to 'test.txt' (%s)\n", cu_cond(res));
	read_res = cu_file_read("test.txt", test_text_tmp, 0, NULL);
	test_text_tmp[sizeof test_text - 1] = '\0';
	fprintf(printfile, COL_BLUE "FILE READ TEST:" COL_RESET " Reading 'test.txt' gives same text back (%s)\n", cu_cond(read_res && strcmp(test_text_tmp, test_text) == 0));
	fprintf(printfile, COL_BLUE "FILE DELETE TEST:" COL_RESET " Deleting 'test.txt' succeeds (%s)\n", cu_cond(cu_file_delete("./test.txt")));

	custr_allocd(&c, exe, 0);
	custr_simplify(&c);
	custr_cd(&c, NULL);
	custr_cd(&c, custr_c("testdir"));
	fprintf(printfile, COL_BLUE "DIRECTORY TEST:" COL_RESET " Creating dir '%s' (%s)\n", c.str, cu_cond(cu_dir_create(c.str)));
	custr_cd(&c, custr_c(batch_name));
	if (!results[2]) {
	fail_exe:
		fprintf(printfile, COL_RED "Skipping rest of file tests.\n" COL_RESET);
		goto fail_dir;
	}

	for (i = res = 0; i < 5; ++i) {
		char content = '0' + (char)i;
		c.str[c.len - 1] = content;
		res += cu_file_write(c.str, &content, CU_FILE_WRITETXT, sizeof content);
	}

	fprintf(printfile, COL_BLUE "BATCH WRITE TEST:" COL_RESET " Created and wrote to %d/%d files in test dir (%s)\n", res, 5, cu_cond(res == 5));

	for (i = res = 0; i < 3; ++i) {
		char content = '0' + (char)i;
		c.str[c.len - 1] = content;
		res += cu_file_delete(c.str);
	}

	fprintf(printfile, COL_BLUE "BATCH DELETE TEST:" COL_RESET " Deleted %d/%d files in test dir (%s)\n", res, 3, cu_cond(res == 3));

	c.str[c.len - 1] = 'd';
	cu_dir_create(c.str);
	custr_cd(&c, custr_c(batch_name));
	c.str[c.len - 1] = 'v';
	cu_dir_create(c.str);
	custr_cd(&c, NULL);
	custr_cd(&c, custr_c(batch_name));
	c.str[c.len - 1] = 'i';
	cu_file_write(c.str, c.str, CU_FILE_WRITETXT, c.len);
	custr_cd(&c, NULL);
	custr_cd(&c, NULL);

	CU_ASSERT(strcmp(c.str + c.len - 8, "testdir" SEP) == 0);
	fprintf(printfile, COL_BLUE "RECURSIVE DELETE TEST:" COL_RESET " Recursively deleting test dir (%s)\n", cu_cond(cu_dir_delete(c.str, 1)));
	custr_clear(&c);

fail_dir:
	fprintf(printfile, COL_BOLD "\nThread tests:\n" COL_RESET);
	fprintf(printfile, "Process ID: %u\n", cu_thread_tid());
	fprintf(printfile, COL_BLUE "MUTEX CREATION TEST:" COL_RESET " Expected success (%s)\n", cu_cond(cu_thread_mutex_init(&mutex)));
	if (!results[2]) goto fail_thread_early;
	thr = cu_thread_create(cu_test_add, NULL);
	fprintf(printfile, COL_BLUE "THREAD CREATON TEST:" COL_RESET " Expected non-zero thread value (%s)\n", cu_cond(thr != 0));
	if (!results[2]) goto fail_thread;
	cu_test_add((void *)(sizeof(void *)));
	fprintf(printfile, "Sleeping until threaded work completes...");
	while (!tid && i++ < 520) cu_thread_sleep(0, 16700);
	if (!tid) fprintf(printfile, " (%s)\n" COL_RED "Took too long.\n" COL_RESET, cu_cond(0));
	else fprintf(printfile, " (%s)\nThread ID: %u\n", cu_cond(1), tid);
	fprintf(printfile, COL_BLUE "MUTEX LOCKING TEST:" COL_RESET " Expected counter to be %d, got %d (%s)\n", N * 2, thread_counter, cu_cond(thread_counter == N * 2));
	fprintf(printfile, COL_BLUE "MUTEX DESTROY TEST:" COL_RESET " Expected success (%s)\n", cu_cond(cu_thread_mutex_destroy(&mutex)));
	fprintf(printfile, COL_BLUE "THREAD JOIN TEST:" COL_RESET " Expected success (%s)\n", cu_cond(cu_thread_join(thr)));
	goto success_thread;
fail_thread:
	cu_thread_mutex_destroy(&mutex);
fail_thread_early:
	fprintf(printfile, COL_RED "Skipping rest of thread tests.\n" COL_RESET);


success_thread:
	fprintf(printfile, COL_BOLD "\nRNG tests:\n" COL_RESET);
	res = (int)cu_rand_cryptographic(&rnd, sizeof rnd);
	fprintf(printfile, COL_BLUE "OS RNG:" COL_RESET " %" CU_U64_FMT " (%s)\n", rnd, cu_cond(res == sizeof rnd));
	fprintf(printfile, COL_BLUE "cu_rand vals:" COL_RESET " %" CU_U64_FMT " %" CU_U64_FMT " %" CU_U64_FMT "\n", cu_rand(), cu_rand(), cu_rand());


	fprintf(printfile, COL_BOLD "\nProgram stats:\n" COL_RESET);
	cu_res_meminfo(&mem);
	fprintf(printfile, "Virtual used by this program: %" CU_UPTR_FMT "B\n", mem.virt_loc_used);
	fprintf(printfile, "Physical used by this program: %" CU_UPTR_FMT "B\n", mem.phys_loc_used);
	cu_timer_fill(&end);
	fprintf(printfile, "Time (excludes sleep time): %fs\n", CU_TIMEDIF_CONV(cu_timer_dif(&start, &end), CU_TIME_SEC));

	fprintf(printfile, "\n%sPassed %d / %d tests.\n" COL_RESET "\n", results[0] == 0 ? COL_GREEN : COL_RED, results[1], results[0] + results[1]);
	return results[0] != 0;
}
