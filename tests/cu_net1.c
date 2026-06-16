#include "tests.h"

#if CU_SETTING_NETWORK_FUNCS

static int client_i, server_i;

static int client_event(cu_net_remote *CU_RESTRICT remote, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	static int hb;
	CU_UNUSED(remote);

	if (ev == CUEVT_HEARTBEAT) {
		if (!hb) EXPECT(cu_net_sendmsg(remote, "data", 5) == CUERR_NONE);
		++hb;
		return 1;
	}

	if (client_i == 0) {
		EXPECT(remote && !remote->ext && (!strcmp(remote->ip, "127.0.0.1") || !strncmp(remote->ip, "192.168.", 8)));
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "data2"));
		free(d);
		EXPECT(n == 6);
		EXPECT(cu_net_sendmsg(remote, "bye", 4) == CUERR_NONE);
	} else if (client_i == 1) {
		EXPECT(ev == CUEVT_DISCONNECT);
		EXPECT0(d);
		EXPECT0(n);
		EXPECT(hb);
	}

	++client_i;
	return 1;
}
static int server_event(struct cu_net_server *CU_RESTRICT server, cu_net_remote *CU_RESTRICT remote, enum cu_net_event ev, void *CU_RESTRICT d, uptr n)
{
	CU_UNUSED(server);
	CU_UNUSED(remote);

	if (server_i == 0) {
		EXPECT(server && remote && !remote->ext && (!strcmp(remote->ip, "127.0.0.1") || !strncmp(remote->ip, "192.168.", 8)));
		EXPECT(ev == CUEVT_CONNECT);
		EXPECT0(d);
		EXPECT(n == 1);
		EXPECT0(remote->ext);
	} else if (server_i == 1) {
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "data"));
		free(d);
		EXPECT(n == 5);
		EXPECT(cu_net_sendmsg(remote, "data2", 6) == CUERR_NONE);
	} else if (server_i == 2) {
		EXPECT(ev == CUEVT_MESSAGE);
		EXPECT(d && !strcmp((const char *)d, "bye"));
		free(d);
		EXPECT(n == 4);
		cu_server_disconnect_client(server, remote);
	} else if (server_i == 3) {
		EXPECT(ev == CUEVT_DISCONNECT);
		EXPECT0(d);
		EXPECT0(n);
		cu_server_close(server);
	}

	++server_i;
	return 1;
}

static CU_THREAD_FUNCTION(client_thread, a)
{
	cu_client_listen((cu_net_remote *)a, client_event, 20);
	EXPECT(client_i == 2);
	cu_client_close((cu_net_remote *)a);
	return CU_THREAD_RETURN_VAL;
}
static CU_THREAD_FUNCTION(server_thread, a)
{
	cu_server_listen((cu_net_server *)a, server_event, -1);
	EXPECT(server_i == 4);
	cu_server_close((cu_net_server *)a);
	return CU_THREAD_RETURN_VAL;
}

TFUNC(cu_net1)
{
	cu_net_server ss;
	cu_net_remote sc;
	cu_thread st, ct;

	EXPECT(cu_net_init());
	EXPECT(cu_net_interfaces(buf, CU_NET_INTERFACE_ANY, 0));
	EXPECT(cu_server_start(&ss, 1234, 2) == CUERR_NONE);
	EXPECT(cu_client_start(&sc, "127.0.0.1", 1234) == CUERR_NONE);
	st = cu_thread_create(server_thread, &ss);
	ct = cu_thread_create(client_thread, &sc);
	cu_thread_join(st);
	cu_thread_join(ct);
	cu_net_terminate();
}

#else
TFUNC(cu_net1) { }
#endif
