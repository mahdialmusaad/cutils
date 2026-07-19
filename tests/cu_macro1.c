#include "tests.h"

TFUNC(cu_macro1)
{
	int c = 1;
	EXPECT(CU_DM_64BIT ? sizeof(void *) == 8 : sizeof(void *) == 4);
	EXPECT(*(char *)&c == CU_ENDIAN_LITTLE);
#if !CU_DM_LL
	EXPECT(0xFFFFFFFFFFFFFFFFUL == CU_U64MAX);
	EXPECT(0x7FFFFFFFFFFFFFFFL == CU_I64MAX);
#endif
}
