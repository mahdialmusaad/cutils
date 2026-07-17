#include "tests.h"

TFUNC(cu_macro1)
{
	int c = 1;
	EXPECT(CU_DM_64BIT ? sizeof(void *) == 8 : sizeof(void *) == 4);
	EXPECT(((u32)(-1)) == CU_U32MAX);
	EXPECT(((u64)(-1)) == CU_U64MAX);
	EXPECT(((uptr)(-1)) == CU_UPTRMAX);
	EXPECT(*(char *)&c == CU_ENDIAN_LITTLE);
}
