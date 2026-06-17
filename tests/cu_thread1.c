#include "tests.h"

#if CU_SETTING_THREAD_FUNCS

static void *thr_shared_p;
static cu_thread_mutex mut;
static cu_thread thr;
static int ecnt, thr_ret;
static u64 mtid, pid;

#define WORKCOUNT 1000

static CU_THREAD_FUNCTION(thrfunc, arg)
{
	int i, c;
	void *s;
	while (!ecnt) cu_thread_sleep(1000000, 0);
	EXPECT(pid == cu_thread_pid());
	EXPECT(mtid != cu_thread_tid());
	EXPECT(thr_shared_p == arg);
	for (i = c = 0; i < WORKCOUNT; ++i) {
		c += cu_thread_mutex_lock(&mut);
		++ecnt;
		c += cu_thread_mutex_unlock(&mut);
	}
	EXPECT(c == WORKCOUNT * 2);
	s = thr_shared_p;
	thr_shared_p = NULL;
	free(s);
	thr_ret = 1;
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_thread1)
{
	int i, c;
	thr_shared_p = malloc(1);
	EXPECT((pid = cu_thread_pid()));
	EXPECT((mtid = cu_thread_tid()));
	EXPECT(cu_thread_mutex_init(&mut));
	EXPECT(thr = cu_thread_create(thrfunc, thr_shared_p));
	for (i = c = 0; i < WORKCOUNT; ++i) {
		c += cu_thread_mutex_lock(&mut);
		++ecnt;
		c += cu_thread_mutex_unlock(&mut);
	}
	while (thr_shared_p) cu_thread_sleep(170000000, 0);
	EXPECT(c == WORKCOUNT * 2);
	EXPECT(ecnt == WORKCOUNT * 2);
	EXPECT(cu_thread_join(thr));
	EXPECT(thr_ret);
	EXPECT(cu_thread_mutex_destroy(&mut));
}

#else
TFUNC(cu_thread1) { }
#endif
