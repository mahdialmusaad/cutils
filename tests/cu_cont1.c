#include "tests.h"

#if CU_SETTING_CONTAINER_FUNCS

static int cu_cont1_eqfunc(const void *key_a, const void *key_b)
{
	return *(const int *)key_a == *(const int *)key_b;
}
static uptr cu_cont1_hashfunc(const void *key)
{
	return (uptr)(*(const int *)key);
}

static int cu_cont1_itercnt;
static void cu_cont1_iterate(const void *key, void *data, void *user)
{
	EXPECT(*(int *)data == 1 || *(int *)data == 2);
	EXPECT(*(const int *)key == 12345 || *(const int *)key == 23456);
	EXPECT0(user);
	++cu_cont1_itercnt;
}

TFUNC(cu_cont1)
{
	int to_add_key = 12345, to_add2_key = 23456, badkey = 2146, goodkey = 2147;
	int to_add = 1, to_add2 = 2, i, c = 0;
	cu_list l;
	cu_hmap h;

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
	cu_list_clear(&l);
	EXPECT(cu_hmap_init(&h, cu_cont1_eqfunc, cu_cont1_hashfunc) == &h);
	EXPECT(cu_hmap_add(&h, &to_add, &to_add_key));
	EXPECT(cu_hmap_add(&h, &to_add2, &to_add2_key));
	EXPECT(*(int *)cu_hmap_find(&h, &to_add_key) == to_add);
	EXPECT(*(int *)cu_hmap_find(&h, &to_add2_key) == to_add2);
	cu_hmap_iterate(&h, NULL, cu_cont1_iterate);
	EXPECT(cu_cont1_itercnt == 2);
	EXPECT(cu_hmap_remove(&h, &to_add_key, 0) == &to_add);
	EXPECT(cu_hmap_remove(&h, &to_add2_key, 0) == &to_add2);
	cu_hmap_clear(&h, 0, 0);
	EXPECT0(cu_hmap_find(&h, &to_add_key));
	EXPECT0(cu_hmap_find(&h, &to_add2_key));

	for (i = 0; i < 2147; ++i) {
		int *data = (int *)malloc(sizeof *data), *key = (int *)malloc(sizeof *key);
		*data = i;
		*key = i + 2147;
		cu_hmap_add(&h, data, key);
	}

	EXPECT0(cu_hmap_find(&h, &badkey));

	for (i = 0; i < 2147; ++i) {
		int key = i + 2147;
		c += *(int *)cu_hmap_find(&h, &key) == i;
	}

	EXPECT(c == 2147);
	cu_hmap_clear(&h, 1, 1);
	EXPECT0(cu_hmap_find(&h, &goodkey));
}

#else
TFUNC(cu_cont1) {}
#endif
