#include "tests.h"

#if CU_SETTING_NETWORK_FUNCS && CU_SETTING_THREAD_FUNCS

static int server_i_2, conns, disconns, disconns_c;
static cu_thread_mutex cu_net2_mutex;
static int err_cnt;

#define NET2CLIENTS 20

static void server_event_2(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT remote, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	CU_UNUSED(d);
	CU_UNUSED(n);

	if (ev == CUEVT_CONNECT) {
		cu_net_sendmsg(remote, "", 1);
		++conns;
	} else if (ev == CUEVT_DISCONNECT) {
		++disconns;
		if (disconns == NET2CLIENTS - err_cnt) cu_net_close(server);
	}

	server_i_2++;
}

static void client_event_2(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT unused, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	CU_UNUSED(unused);
	CU_UNUSED(d);
	CU_UNUSED(n);

	if (ev == CUEVT_MESSAGE) {
		free(d);
		cu_net_close(server);
	} else if (ev == CUEVT_DISCONNECT) {
		cu_thread_mutex_lock(&cu_net2_mutex);
		++disconns_c;
		cu_thread_mutex_unlock(&cu_net2_mutex);
	}
}

static CU_THREAD_FUNCTION(client_thread_2, a)
{
	cu_net_remote s;
	CU_UNUSED(a);
	if (!cu_client_listen(&s, "127.0.0.1", "4321", NULL, CU_NET_RETRYVAL(5, 300), 0, client_event_2, -1)) {
		cu_thread_mutex_lock(&cu_net2_mutex);
		++err_cnt;
		cu_thread_mutex_unlock(&cu_net2_mutex);
		EXPECT(0);
	}
	return CU_THREAD_RETURN_VAL;
}
static CU_THREAD_FUNCTION(server_thread_2, a)
{
	cu_net_remote s;
	CU_UNUSED(a);
	if (!cu_server_listen(&s, "4321", NULL, 0, NET2CLIENTS, server_event_2, -1)) EXPECT(0);
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_net2)
{
	cu_thread st, ct[NET2CLIENTS];
	int i;

	cu_net_init();
	cu_thread_mutex_init(&cu_net2_mutex);
	st = cu_thread_create(server_thread_2, NULL);
	for (i = 0; i < NET2CLIENTS; ++i) ct[i] = cu_thread_create(client_thread_2, NULL);
	cu_thread_join(st);
	for (i = 0; i < NET2CLIENTS; ++i) cu_thread_join(ct[i]);
	EXPECT(server_i_2 == NET2CLIENTS * 2);
	EXPECT(disconns_c == NET2CLIENTS && disconns == NET2CLIENTS);
	EXPECT(conns == NET2CLIENTS);
	EXPECT0(err_cnt);
	cu_thread_mutex_destroy(&cu_net2_mutex);
	cu_net_terminate();
}

#else
TFUNC(cu_net2) { }
#endif
