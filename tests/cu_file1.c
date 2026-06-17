#include "tests.h"

#if CU_SETTING_FILE_FUNCS

#define s_strlen(C) (C ? strlen(C) : 0)

TFUNC(cu_file1)
{
	cu_file_info f;
	time_t bf = time(NULL);
	char *pb, **dl, **dlfn;
	int c, c2;
	uptr u;

	memset(&f, 0, sizeof f);

	EXPECT((pb = cu_file_exe_path(*e_argv, &u)));
	EXPECT(u > s_strlen(pb));
	free(pb);
	EXPECT(cu_dir_create("tdir"));
	EXPECT(cu_dir_exists("tdir"));
	EXPECT(cu_file_write("tdir" Q "tfile", "data", CU_FILE_WRITETXT, 4));
	EXPECT(cu_file_exists("tdir" Q "tfile"));
	EXPECT(cu_file_read("tdir" Q "tfile", buf, 0, &u));
	EXPECT0(strncmp(buf, "data", 4));
	EXPECT(u == 4);
	EXPECT(cu_file_getinfo("tdir" Q "tfile", &f));
	EXPECT(f.fsize_bytes >= 4);
	EXPECT(f.mod_time >= bf);
	EXPECT(dl = cu_dir_list("tdir", 0, &c));
	EXPECT(dlfn = cu_dir_list("tdir", 1, &c2));
	EXPECT(strcmp(*dl, "tfile") == 0);
	EXPECT(strcmp(*dlfn, "tdir" Q "tfile") == 0);
	EXPECT(c == 1);
	cu_dir_close(dl, c);
	cu_dir_close(dlfn, c2);
	EXPECT(cu_file_delete("tdir" Q "tfile"));
	EXPECT0(cu_file_exists("tdir" Q "tfile"));
	EXPECT(cu_dir_delete("tdir", 0));
	EXPECT0(cu_dir_exists("tdir"));
	EXPECT0(cu_dir_list("tdir", 0, &c));
}

#else
TFUNC(cu_file1) { }
#endif
