#include "tests.h"

#if CU_SETTING_NETWORK_FUNCS && CU_SETTING_THREAD_FUNCS

static int client_i, server_i;

static void client_event(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT unused, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	static int hb;
	char ipbuf[CU_NET_IPADDR_LEN];
	CU_UNUSED(unused);

	if (ev == CUEVT_HEARTBEAT) {
		if (!hb) {
			EXPECT(cu_net_sendmsg(server, "data", 5) == 1);
		}
		++hb;
		return;
	}

	if (client_i == 0) {
		EXPECT(server && !server->user && cu_net_ipinfo(server, ipbuf));
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "data2"));
		free(d);
		EXPECT(n == 6);
		EXPECT(cu_net_sendmsg(server, "bye", 4) == 1);
	} else if (client_i == 1) {
		EXPECT(ev == CUEVT_DISCONNECT);
		EXPECT0(d);
		EXPECT0(n);
		EXPECT(hb);
	}

	++client_i;
}
static void server_event(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT client, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	static cu_net_remote *saved_client = NULL;
	char ipbuf[CU_NET_IPADDR_LEN];
	CU_UNUSED(server);
	CU_UNUSED(client);

	if (server_i == 0) {
		EXPECT(server && client && !client->user && cu_net_ipinfo(server, ipbuf));
		EXPECT(ev == CUEVT_CONNECT);
		EXPECT0(d);
		EXPECT(n == 1);
		saved_client = client;
	} else if (server_i == 1) {
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "data"));
		free(d);
		EXPECT(n == 5);
		EXPECT(cu_net_sendmsg(client, "data2", 6) == 1);
	} else if (server_i == 2) {
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "bye"));
		free(d);
		EXPECT(n == 4);
		cu_net_close(saved_client);
	} else if (server_i == 3) {
		EXPECT(ev == CUEVT_DISCONNECT);
		EXPECT(client == saved_client);
		EXPECT0(d);
		EXPECT0(n);
		cu_net_close(server);
	} else EXPECT(0);

	++server_i;
}

static CU_THREAD_FUNCTION(client_thread, a)
{
	cu_net_remote s;
	CU_UNUSED(a);
	EXPECT(cu_client_listen(&s, "127.0.0.1", "1234", NULL, CU_NET_RETRYVAL(5, 1), 0, client_event, 100));
	EXPECT(client_i == 2);
	return CU_THREAD_RETURN_VAL;
}
static CU_THREAD_FUNCTION(server_thread, a)
{
	cu_net_remote s;
	CU_UNUSED(a);
	EXPECT(cu_server_listen(&s, "1234", NULL, 0, 1, server_event, -1));
	EXPECT(server_i == 4);
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_net1)
{
	cu_thread st, ct;
	EXPECT(cu_net_interfaces(buf, CU_NET_INTERFACE_ANY, 0));
	EXPECT(cu_net_init());
	st = cu_thread_create(server_thread, NULL);
	ct = cu_thread_create(client_thread, NULL);
	cu_thread_join(st);
	cu_thread_join(ct);
	cu_net_terminate();
}

#else
TFUNC(cu_net1) { }
#endif
