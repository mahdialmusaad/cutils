#include "tests.h"

#if CU_SETTING_THREAD_FUNCS

static int *poolwork;
static int *splitwork;
static cu_thread_mutex poolmutex;
static int pooldone;

#define WORKMULT 10

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
	for (; a->start_index < a->end_index; ++a->start_index) splitwork[a->start_index] = (int)((uptr)a->thread_arg);
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_thread2)
{
	cu_thread_pool pool;
	int nthreads = cu_thread_count(), i, c;
	int *workcounts = (int *)malloc(sizeof *workcounts * (size_t)nthreads);
	void **eacharg = (void **)malloc(sizeof *eacharg * (size_t)nthreads);
	for (i = 0; i < nthreads; ++i) { eacharg[i] = (void *)((uptr)i); workcounts[i] = WORKMULT + (i < 2); }
	splitwork = (int *)malloc(sizeof *splitwork * (size_t)nthreads * WORKMULT + (sizeof *splitwork * 3));
	splitwork[nthreads * WORKMULT + 2] = nthreads;
	EXPECT(cu_thread_split(splitjob, (u64)(nthreads * WORKMULT) + 2, eacharg, nthreads));
	for (i = 0; i < nthreads * WORKMULT + 2; ++i) --workcounts[splitwork[i]];
	for (i = c = 0; i < nthreads; ++i) c += !workcounts[i];
	EXPECT(c == nthreads);
	EXPECT(splitwork[nthreads * WORKMULT + 2] == nthreads);
	cu_thread_mutex_init(&poolmutex);
	poolwork = (int *)calloc((size_t)nthreads * WORKMULT, sizeof *poolwork);
	EXPECT(cu_thread_pool_init(&pool, nthreads));
	for (i = c = 0; i < pool.nthreads * WORKMULT; ++i) c += cu_thread_pool_add(&pool, pooljob, (void *)((uptr)i));
	EXPECT(c == i);
	while (pooldone < pool.nthreads * WORKMULT) cu_thread_sleep(17000000, 0);
	for (i = c = 0; i < pool.nthreads; ++i) c += poolwork[i] == 1000;
	EXPECT(c == i);
	free(workcounts);
	free(poolwork);
	free(splitwork);
	free(eacharg);
	cu_thread_mutex_destroy(&poolmutex);
	cu_thread_pool_destroy(&pool);
}

#else
TFUNC(cu_thread2) { }
#endif
