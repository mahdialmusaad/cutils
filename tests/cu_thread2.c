#include "tests.h"

#if CU_SETTING_THREAD_FUNCS

static volatile int *poolwork;
static char *splitwork;
static cu_thread_mutex poolmutex;
static int pooldone;

#define WORKMULT 1

static CU_THREAD_FUNCTION(pooljob, arg)
{
	int id = (int)((uptr)arg), i;
	for (i = 0; i < 1000; ++i) poolwork[id]++;
	cu_thread_mutex_lock(&poolmutex);
	++pooldone;
	cu_thread_mutex_unlock(&poolmutex);
	return CU_THREAD_RETURN_VAL;
}

static CU_THREAD_FUNCTION(splitjob, arg)
{
	cu_thread_split_arg *a = (cu_thread_split_arg *)arg;
	EXPECT(a->thread_arg == (void *)0x2);
	for (; a->start_index < a->end_index; ++a->start_index) splitwork[a->start_index] = (char)1;
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_thread2)
{
	cu_thread_pool pool;
	int nthreads = cu_thread_count(), i, c;
	void **eacharg = malloc(sizeof(void *) * (size_t)nthreads);
	for (i = 0; i < nthreads; ++i) eacharg[i] = (void *)0x2;
	splitwork = (char *)malloc(nthreads * WORKMULT);
	EXPECT(cu_thread_split(splitjob, (u64)(nthreads * WORKMULT), eacharg, nthreads));
	cu_thread_mutex_init(&poolmutex);
	poolwork = (volatile int *)calloc((size_t)nthreads * WORKMULT, sizeof *poolwork);
	EXPECT(cu_thread_pool_init(&pool, nthreads));
	for (i = c = 0; i < pool.nthreads * WORKMULT; ++i) c += cu_thread_pool_add(&pool, pooljob, (void *)((uptr)i));
	EXPECT(c == i);
	while (pooldone < pool.nthreads * WORKMULT) cu_thread_sleep(17000000, 0);
	for (i = c = 0; i < pool.nthreads; ++i) c += poolwork[i] == 1000;
	EXPECT(c == i);
	free((void *)poolwork);
	free(splitwork);
	free(eacharg);
	cu_thread_mutex_destroy(&poolmutex);
	cu_thread_pool_destroy(&pool);
}

#else
TFUNC(cu_thread2) { }
#endif
