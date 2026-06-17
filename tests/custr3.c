#include "tests.h"

#if CU_SETTING_STRING_FUNCS

TFUNC(custr3)
{
	custr cs = CUSTR_EMPTY, *c = &cs;

	EXPECT(custr_fmt(c, "%s %d %s %d %s %.1f", "CUSTR4.C", 1, "STRING", 2, "FMT", 3.0));
	if (!c->str) return;
	EXPECT0(strcmp(c->str, "CUSTR4.C 1 STRING 2 FMT 3.0"));
	EXPECT(c->cap == 32);
	EXPECT(custr_find(c, 0, '3', 0) == 24);
	EXPECT(custr_find(c, 0, '.', 1) == 25);
	EXPECT(custr_find(c, 0, '.', -1) == 25);
	EXPECT(custr_find(c, 2, '.', -1) == 6);
	EXPECT(custr_find(c, 0, '.', -2) == 6);
	EXPECT(custr_find(c, 0, '.', -3) == CU_UPTRMAX);
	EXPECT(custr_find(c, 0, '.', 2) == CU_UPTRMAX);
	EXPECT(custr_findnot(c, 0, 'C', 0) == 1);
	EXPECT(custr_findnot(c, 0, '0', -1) == 25);
	EXPECT0(((custr_replace(c, 0, ' ', '\0')), (strcmp(c->str, "CUSTR4.C1STRING2FMT3.0"))));
	EXPECT0(((custr_replace(c, 0, '.', '\0')), (strcmp(c->str, "CUSTR4C1STRING2FMT30"))));
	EXPECT0(((custr_replace(c, 0, 'C', 'D')), (strcmp(c->str, "DUSTR4D1STRING2FMT30"))));
	EXPECT0(((custr_replace(c, 0, '\0', '4')), (strcmp(c->str, "DUSTR4D1STRING2FMT30"))));
	EXPECT(custr_replacesub(c, 0, "STR", "FMT"));
	EXPECT0(strcmp(c->str, "DUFMT4D1FMTING2FMT30"));
	EXPECT(custr_replacesub(c, 0, "FMT", NULL));
	EXPECT0(strcmp(c->str, "DU4D1ING230"));
	EXPECT(custr_replacesub(c, 0, "ING", "ABCDEF"));
	EXPECT0(strcmp(c->str, "DU4D1ABCDEF230"));
	EXPECT(custr_replacesub(c, 0, "ABCDEF", "ING"));
	EXPECT0(strcmp(c->str, "DU4D1ING230"));
	custr_clear(c);
}

#else
TFUNC(custr3) { }
#endif
