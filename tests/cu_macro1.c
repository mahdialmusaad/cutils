#include "tests.h"

TFUNC(cu_macro1)
{
	int c = 1;

	EXPECT(sizeof(void *) == CU_PTR_BYTES);
	EXPECT(((u32)(-1)) == CU_U32MAX);
	EXPECT(((u64)(-1)) == CU_U64MAX);
	EXPECT(((uptr)(-1)) == CU_UPTRMAX);
	EXPECT(*(char *)&c == CU_ENDIAN_LITTLE);
}
