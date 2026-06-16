#include "tests.h"

#if CU_SETTING_STRING_FUNCS

TFUNC(custr2)
{
	custr cs, *c = &cs, ss = CUSTR_EMPTY;

	custr_create(c, "CUSTR2.C STRING");
	EXPECT(custr_insert(c, 8, " INSERT"));
	EXPECT0(strcmp(c->str, "CUSTR2.C INSERT STRING"));
	EXPECT(c->cap == 32);
	EXPECT(custr_append(c, " APPEND"));
	EXPECT0(strcmp(c->str, "CUSTR2.C INSERT STRING APPEND"));
	EXPECT(c->cap == 32);
	EXPECT(custr_insert(c, 0, "PREPEND "));
	EXPECT0(strcmp(c->str, "PREPEND CUSTR2.C INSERT STRING APPEND"));
	EXPECT(c->cap == 64);
	EXPECT0(custr_insert(c, c->len + 1, "ERROR_INSERT"));
	EXPECT(custr_sub(c, &ss, 17, c->len));
	EXPECT0(strcmp(ss.str, "INSERT STRING APPEND"));
	EXPECT(ss.cap == 32);
	EXPECT0(((custr_cut(c, 16, 14)), (strcmp(c->str, "PREPEND CUSTR2.C APPEND"))));
	EXPECT(((custr_cut(c, 0, 1000)), (!*c->str)));
	EXPECT(c->len == 0);
	EXPECT(c->cap == 64);
	EXPECT(custr_optimize(c));
	EXPECT(c->cap == 8);
	custr_clear(c);
	EXPECT(custr_tosub(&ss, 7, 9));
	EXPECT0(strcmp(ss.str, "STR"));
	custr_clear(&ss);
	EXPECT(((custr_create(c, "STRING")), (custr_tosub(c, 2, 1000))));
	EXPECT0(strcmp(c->str, "RING"));
	EXPECT0(((custr_shrinkto(c, 2)), (strcmp(c->str, "RI"))));
	EXPECT0(custr_shrinkto(c, c->len));
	EXPECT(custr_tosub(c, 1, 1));
	EXPECT0(strcmp(c->str, "I"));
	custr_clear(c);
}

#else
TFUNC(custr2) { }
#endif
