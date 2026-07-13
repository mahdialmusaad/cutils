#include "tests.h"

#if CU_SETTING_CONTAINER_FUNCS

TFUNC(cu_cont1)
{
	int to_add = 1, to_add2 = 2, c = 0;
	cu_list l;

	EXPECT(cu_list_init(&l, sizeof(int)) == &l);
	EXPECT(l.elem == sizeof(int));
	EXPECT0(l.data);
	EXPECT0(l.len);
	EXPECT0(l.cap);
	EXPECT(cu_list_add(&l, &to_add));
	EXPECT(*(int *)cu_list_at(&l, 0) == to_add);
	EXPECT(cu_list_atsf(&l, l.len) == NULL);
	EXPECT(l.cap >= l.len);
	EXPECT(l.len == 1);
	EXPECT(cu_list_insert(&l, &to_add2, 1, 0));
	EXPECT(l.len == 2);
	EXPECT(l.cap >= l.len);
	EXPECT(*(int *)cu_list_at(&l, 0) == to_add2);
	EXPECT(*(int *)cu_list_at(&l, 1) == to_add);
	EXPECT(cu_list_has(&l, &to_add2));
	EXPECT0(cu_list_has(&l, &c));
	cu_list_clear(&l);
}

#else
TFUNC(cu_cont1) {}
#endif
