#include "tests.h"

#if CU_SETTING_THREAD_FUNCS

static void *thr_shared_p;
static cu_thread_mutex mut;
static cu_thread_cond cond;
static cu_thread thr;
static int ecnt, thr_ret;
static u32 mtid, pid;
static int thr_finish;

static CU_THREAD_FUNCTION(thrfunc, arg)
{
	int i;
	void *s;
	EXPECT(pid == cu_thread_pid());
	EXPECT(mtid != cu_thread_tid());
	EXPECT(thr_shared_p == arg);
	for (i = 0; i < 5; ++i) {
		EXPECT(cu_thread_mutex_lock(&mut));
		++ecnt;
		EXPECT(cu_thread_mutex_unlock(&mut));
	}
	s = thr_shared_p;
	thr_shared_p = NULL;
	free(s);
	cu_thread_sleep(1700000000, 0);
	thr_ret = 1;
	return CU_THREAD_RETURN_VAL;
}

static CU_THREAD_FUNCTION(thrfunc2, arg)
{
	EXPECT0(arg);
	cu_thread_sleep(50000000, 0);
	EXPECT(cu_thread_mutex_lock(&mut));
	thr_finish = 1;
	EXPECT(cu_thread_cond_signal(&cond));
	EXPECT(cu_thread_mutex_unlock(&mut));
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_thread1)
{
	int i, c;
	thr_shared_p = malloc(1);
	EXPECT((pid = cu_thread_pid()));
	EXPECT((mtid = cu_thread_tid()));
	EXPECT(cu_thread_mutex_init(&mut));
	EXPECT(cu_thread_cond_init(&cond));
	EXPECT(thr = cu_thread_create(thrfunc, thr_shared_p));
	for (i = 0; i < 5; ++i) {
		EXPECT(cu_thread_mutex_lock(&mut));
		++ecnt;
		EXPECT(cu_thread_mutex_unlock(&mut));
	}
	for (c = 0; thr_shared_p; ++c) cu_thread_sleep(170000000, 0);
	EXPECT(ecnt == 10);
	EXPECT(cu_thread_join(thr));
	EXPECT(thr_ret);
	EXPECT(cu_thread_mutex_lock(&mut));
	EXPECT(thr = cu_thread_create(thrfunc2, NULL));
	for (c = i = 0; !thr_finish; ++i) {
		c += cu_thread_cond_wait(&cond, &mut);
	}
	EXPECT(c == i);
	EXPECT(cu_thread_mutex_unlock(&mut));
	EXPECT(cu_thread_join(thr));
	EXPECT(cu_thread_cond_destroy(&cond));
	EXPECT(cu_thread_mutex_destroy(&mut));
}

#else
TFUNC(cu_thread1) { }
#endif
