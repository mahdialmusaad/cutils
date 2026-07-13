#include "tests.h"

#if CU_SETTING_CONTAINER_FUNCS

static int cu_cont2_eqfunc(const void *key_a, const void *key_b)
{
	return *(const int *)key_a == *(const int *)key_b;
}
static uptr cu_cont2_hashfunc(const void *key)
{
	return (uptr)(*(const int *)key);
}

static int cu_cont2_itercnt;
static void cu_cont2_iterate(const void *key, void *data, void *user)
{
	EXPECT(*(int *)data >= 1 || *(int *)data <= 4);
	EXPECT(*(const int *)key == 12345 || *(const int *)key == 23456 || *(const int *)key == 12345 + 256 || *(const int *)key == 34567);
	EXPECT0(user);
	++cu_cont2_itercnt;
}

TFUNC(cu_cont2)
{
	int to_add_key = 12345, to_add2_key = 23456, to_add3_key = 12345 + 256, to_add4_key = 34567;
	int to_add = 1, to_add2 = 2, to_add3 = 3, to_add4 = 4, i, c = 0;
	int badkey = 2146, goodkey = 2147;
	cu_hmap h, h2;

	EXPECT(cu_hmap_init(&h, cu_cont2_eqfunc, cu_cont2_hashfunc) == &h);
	EXPECT(cu_hmap_add(&h, &to_add, &to_add_key));
	EXPECT(cu_hmap_add(&h, &to_add2, &to_add2_key));
	EXPECT(*(int *)cu_hmap_find(&h, &to_add_key) == to_add);
	EXPECT(*(int *)cu_hmap_find(&h, &to_add2_key) == to_add2);
	cu_hmap_iterate(&h, NULL, cu_cont2_iterate);
	EXPECT(cu_cont2_itercnt == 2);
	EXPECT(cu_hmap_init(&h2, cu_cont2_eqfunc, cu_cont2_hashfunc) == &h2);
	EXPECT(cu_hmap_add(&h2, &to_add3, &to_add3_key));
	EXPECT(cu_hmap_add(&h2, &to_add4, &to_add4_key));
	cu_hmap_combine(&h, &h2);
	EXPECT(cu_hmap_find(&h, &to_add3_key));
	EXPECT(cu_hmap_find(&h, &to_add4_key));
	cu_hmap_iterate(&h, NULL, cu_cont2_iterate);
	EXPECT(cu_cont2_itercnt == 6);
	EXPECT(cu_hmap_remove(&h, &to_add_key, 0) == &to_add);
	EXPECT(cu_hmap_remove(&h, &to_add3_key, 0) == &to_add3);
	EXPECT0(cu_hmap_find(&h, &to_add_key));
	EXPECT0(cu_hmap_find(&h, &to_add3_key));
	EXPECT(cu_hmap_clear(&h, 0, 0) == &h);
	EXPECT0(cu_hmap_find(&h, &to_add2_key));
	EXPECT0(cu_hmap_find(&h, &to_add4_key));

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
TFUNC(cu_cont2) {}
#endif
