#include "tests.h"

#if CU_SETTING_FILE_FUNCS

#define s_strlen(C) (C ? strlen(C) : 0)

TFUNC(cu_file1)
{
	cu_file_info f;
	time_t bf = time(NULL);
	char *pb;
	uptr u;

	memset(&f, 0, sizeof f);

	EXPECT((pb = cu_file_exe_path(*e_argv, &u)));
	EXPECT(u > s_strlen(pb));
	EXPECT(cu_dir_create("tdir"));
	EXPECT(cu_dir_exists("tdir"));
	EXPECT(cu_dir_delete("tdir", 0));
	EXPECT0(cu_dir_exists("tdir"));
	EXPECT(cu_file_write("tfile", "data", CU_FILE_WRITETXT, 4));
	EXPECT(cu_file_exists("tfile"));
	EXPECT(cu_file_read("tfile", buf, 0, &u));
	EXPECT0(strncmp(buf, "data", 4));
	EXPECT(u == 4);
	EXPECT(cu_file_getinfo("tfile", &f));
	EXPECT(f.fsize_bytes >= 4);
	EXPECT(f.mod_time >= bf);
	EXPECT(cu_file_delete("tfile"));
	EXPECT0(cu_file_exists("tfile"));
}

#else
TFUNC(cu_file1) { }
#endif
