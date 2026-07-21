#include "tests.h"

#if CU_SETTING_TIME_FUNCS

TFUNC(cu_time1)
{
	cu_ctime ct, epoch;
	cu_time_now(&ct);
	cu_time_date(&epoch, 0);
	EXPECT0(epoch.m.tm_hour - ((epoch.utcdif / 3600) % 24));
	EXPECT0(epoch.m.tm_min - ((epoch.utcdif / 60) % 60));
	EXPECT0(epoch.m.tm_sec - ((epoch.utcdif / 60) % 60));
}

#else
TFUNC(cu_time1) {}
#endif
