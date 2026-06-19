#include "tests.h"

#if CU_SETTING_TIME_FUNCS

TFUNC(cu_time1)
{
	volatile int i;
	cu_ctime ct, epoch;
	cu_timer start, end;

	cu_timer_fill(&start);
	cu_time_now(&ct);
	EXPECT(ct.isdst != -1);
	cu_time_date(&epoch, 0);
	EXPECT0(epoch.hour - ((epoch.utcdif / 3600) % 24));
	EXPECT0(epoch.minute - ((epoch.utcdif / 60) % 60));
	EXPECT0(epoch.second - ((epoch.utcdif / 60) % 60));

	for (i = 0; i < 1000000; ++i);

	cu_timer_fill(&end);
	EXPECT(cu_timer_dif(&start, &end));
}

#else
TFUNC(cu_time1) {}
#endif
