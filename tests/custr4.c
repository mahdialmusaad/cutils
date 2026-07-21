#include "tests.h"

#if CU_SETTING_STRING_FUNCS

TFUNC(custr4)
{
	custr cs, *c = &cs;

	custr_create(c, FS(Q "abc" Q "123" Q "def" Q "456"));
	EXPECT(custr_cd(c, "cd1"));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q "cd1")));
	EXPECT(custr_cd(c, Q "cd2" Q));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q "cd1" Q "cd2" Q)));
	EXPECT(custr_cd(c, Q "cd3" Q2 "." Q3 "." Q2 "cd4" Q3 Q3 ".." Q3 "cd4" Q ".."));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q "cd1" Q "cd2" Q "cd3" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q "cd1" Q "cd2" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q "cd1" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q "456" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q "def" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q "123" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q "abc" Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q)));
	EXPECT(custr_cd(c, NULL));
	EXPECT0(strcmp(c->str, FS(Q)));
	custr_clear(c);
}

#else
TFUNC(custr4) { }
#endif
