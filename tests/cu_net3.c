#include "tests.h"

#if CU_SETTING_NETWORK_FUNCS && CU_SETTING_THREAD_FUNCS

#define NET3CLIENTS 10
#define NET3MSGS 3

static int server_ready;
static CU_ATTRIB_USED int server3_msgs;
static const char server_reply[] = "Reply from server.";

static int nleft;
static cu_thread_mutex nleftmutex;

struct client_counters
{
	int hb;
};

static void server_event_3(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT remote, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	if (ev == CUEVT_HEARTBEAT) {
		if (nleft == NET3CLIENTS) {
			cu_net_close(server);
			return;
		}
		if (server_ready) return;
		EXPECT(server_ready = 1);
		EXPECT(server);
		EXPECT0(remote);
		EXPECT0(d);
		EXPECT0(n);
		return;
	} else if (ev !=  CUEVT_MESSAGE) EXPECT(0);

	cu_net_sendmsg(remote, server_reply, sizeof server_reply);
	++server3_msgs;
	free(d);
}

static void client_event_3(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT unused, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	struct client_counters *cc = (struct client_counters *)server->user;
	CU_UNUSED(unused);
	CU_UNUSED(n);

	if (ev == CUEVT_HEARTBEAT) {
		u64 zerodata[10];
		memset(zerodata, 0, sizeof zerodata);
		if (cc->hb < NET3MSGS) {
			++cc->hb;
			cu_net_sendmsg(server, zerodata, sizeof zerodata);
		} else {
			cu_thread_mutex_lock(&nleftmutex);
			++nleft;
			cu_thread_mutex_unlock(&nleftmutex);
			cu_net_close(server);
		}
	} else if (ev == CUEVT_MESSAGE) {
		CU_ASSERT(!strcmp((char *)d, server_reply));
		free(d);
	} else EXPECT(0);
}

static CU_THREAD_FUNCTION(client_thread_3, a)
{
	cu_net_remote s;
	while (!server_ready) cu_thread_sleep(1000000, 0);
	cu_client_listen(&s, "127.0.0.1", "2147", a, CU_NET_RETRYVAL(5, 300), CU_NETMODE_UDP, client_event_3, 10);
	return CU_THREAD_RETURN_VAL;
}
static CU_THREAD_FUNCTION(server_thread_3, a)
{
	cu_net_remote s;
	CU_UNUSED(a);
	cu_server_listen(&s, "2147", NULL, CU_NETMODE_UDP, NET3CLIENTS, server_event_3, 10);
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_net3)
{
	cu_thread st, ct[NET3CLIENTS];
	struct client_counters ccs[NET3CLIENTS];
	int i, c = 0;
	cu_net_init();
	cu_thread_mutex_init(&nleftmutex);
	memset(&ccs, 0, sizeof ccs);
	for (i = 0; i < NET3CLIENTS; ++i) ct[i] = cu_thread_create(client_thread_3, ccs + i);
	st = cu_thread_create(server_thread_3, NULL);
	for (i = 0; i < NET3CLIENTS; ++i) {
		cu_thread_join(ct[i]);
		c += ccs[i].hb == NET3MSGS;
	}
	/* printf("lost %d ", (NET3CLIENTS * NET3MSGS) - server3_msgs);
	fflush(stdout); */
	cu_thread_join(st);
	cu_net_terminate();
	EXPECT(c == NET3CLIENTS);
}

#else
TFUNC(cu_net3) { }
#endif
