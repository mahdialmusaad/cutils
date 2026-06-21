#include "tests.h"

#if CU_SETTING_STRING_FUNCS

TFUNC(custr1)
{
	custr cs, *c = &cs, al;
	char *astr = (char *)malloc(5);

	EXPECT(custr_create(c, "CUSTR1.C STRING"));
	EXPECT(c->str);
	EXPECT(c->len == 15);
	EXPECT(c->cap == 16);
	EXPECT0(strcmp(c->str, "CUSTR1.C STRING"));
	EXPECT(custr_count(c, 'C') == 2);
	EXPECT(custr_count(c, 'A') == 0);
	EXPECT(custr_count(c, ' ') == 1);
	EXPECT(custr_count(c, '\0') == 0);
	EXPECT(((c->str[1] = '\0'), (custr_count(c, '\0') == 1)));
	EXPECT(custr_countsub(c, "ST") == 2);
	EXPECT(custr_countsub(c, "STR") == 2);
	EXPECT(custr_countsub(c, "C.") == 0);
	EXPECT(custr_countsub(c, ".C") == 1);
	astr[0] = 'a'; astr[1] = 'b'; astr[2] = 'c'; astr[3] = 'd'; astr[4] = '\0';
	EXPECT(custr_allocd(&al, astr, 5) == &al);
	EXPECT0(strcmp(al.str, "abcd"));
	EXPECT(al.len == 4);
	EXPECT(al.cap == 5);
	EXPECT(custr_reserve(&al, 17));
	EXPECT(al.cap == 32);
	EXPECT0(strcmp(al.str, "abcd"));
	EXPECT(custr_clear(c) == c);
	EXPECT0(c->len);
	EXPECT0(c->cap);
	EXPECT0(c->str);
	EXPECT(custr_copy(&al, c));
	EXPECT0(strcmp(c->str, "abcd"));
	EXPECT(c->len == 4);
	EXPECT(c->cap == 5);
	custr_clear(&al);
	custr_clear(c);
}

#else
TFUNC(custr1) { }
#endif
