#include "../cutils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int client_event(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT unused, enum cu_net_event event_type, void *CU_RESTRICT data, uptr n)
{
	CU_UNUSED(unused);
	CU_UNUSED(server);

	if (event_type == CUEVT_MESSAGE) {
		printf("%.*s\n", (int)n, (char *)data);
		free(data);
	} else if (event_type == CUEVT_DISCONNECT) printf("[ Client ] Disconnected.\n");
	return 1;
}
static CU_THREAD_FUNCTION(client_scanf, server)
{
	while (1) {
		char msg[1024], *res = fgets(msg, 1024, stdin);
		if (res && strlen(msg) && msg[strlen(msg) - 1] == '\n') msg[strlen(msg) - 1] = '\0';
		if (!strlen(msg)) continue;
		if (!res || !strcmp(msg, "exit") || !strcmp(msg, "EXIT")) break;
		if (res && !cu_net_sendmsg((cu_net_remote *)server, msg, strlen(msg) + 1)) fprintf(stderr, "Failed to send message to server.");
	}
	cu_net_close((cu_net_remote *)server);
	return CU_THREAD_RETURN_VAL;
}

static void client_main(const char *CU_RESTRICT addr, const char *CU_RESTRICT port)
{
	cu_net_remote server;
	cu_thread scanf_thr;
	char ipbuf[CU_NET_IPADDR_LEN];

	if (!(scanf_thr = cu_thread_create(client_scanf, &server))) fprintf(stderr, "Failed to create input thread.");
	else if (!cu_client_listen(&server, addr, port, NULL, CU_NET_RETRYVAL(3, 1000), 0, client_event, 1000)) fprintf(stderr, "Failed to start client.\n");
	else printf("Ending connection to '%s' (IP %s).\n", addr, cu_net_ipinfo(&server, ipbuf));
}


typedef struct client {
	char *name;
} client;
static client *clients;
static uptr nclients;

static int server_event(cu_net_remote *CU_RESTRICT server, cu_net_remote *CU_RESTRICT client_rem, enum cu_net_event event_type, void *CU_RESTRICT data, uptr n)
{
	client *c = client_rem ? (client *)client_rem->user : NULL;
	char ipbuf[CU_NET_IPADDR_LEN];
	if (client_rem) cu_net_ipinfo(client_rem, ipbuf);

	if (event_type == CUEVT_MESSAGE) {
		uptr u, displayable = 0;
		char *strdata = (char *)data;

		for (u = 0; u < n - 1; ++u) {
			if (strdata[u] && (strdata[u] <= ' ' || strdata[u] > '~')) strdata[u] = ' ';
			else ++displayable;
		}
		if (displayable < 1) return 1;

		if (!c->name) {
			custr notify = CUSTR_EMPTY;
			uptr i;

			if (n >= 31) strdata[30] = '\0';

			for (i = 0; i < nclients; ++i) {
				const char duplicate_msg[] = "[ Server ] That name is already in use.";
				if (!clients[i].name || strcmp(c->name, strdata)) continue;
				cu_net_sendmsg(client_rem, duplicate_msg, sizeof duplicate_msg);
				return 1;
			}

			printf("Client %s says their name is '%s'.\n", ipbuf, strdata);
			c->name = strdata;

			if (nclients == 1) custr_create(&notify, "[ Server ] Welcome, you are the only person connected.");
			else {
				custr_create(&notify, "[ Server ] Welcome, the other active user(s) are:");
				for (i = 0; i < nclients - 1; ++i) {
					custr addfmt = CUSTR_EMPTY;
					if (!clients[i].name) continue;
					custr_fmt(&addfmt, " '%s'", clients[i].name);
					custr_append(&notify, addfmt.str);
					custr_clear(&addfmt);
				}
			}

			cu_net_sendmsg(client_rem, notify.str, notify.len + 1);
			custr_clear(&notify);
			custr_fmt(&notify, "[ Server ] %s has joined.", strdata);
			cu_server_broadcast(server, notify.str, notify.len + 1, &client_rem, 1);
			custr_clear(&notify);
		} else {
			custr msg = CUSTR_EMPTY;
			custr_fmt(&msg, "%s: %s", c->name, strdata);
			printf("Broadcasting %s's %uB message to %d others: \"%.50s\"%s\n", c->name, (unsigned)n, (int)nclients - 1, strdata, n > 50 ? "..." : "");
			free(strdata);
			cu_server_broadcast(server, msg.str, msg.len + 1, &client_rem, 1);
			custr_clear(&msg);
		}
	} else if (event_type == CUEVT_CONNECT) {
		const char name_msg[] = "[ Server ] Enter a username to be identified by.";
		clients = (client *)realloc(clients, sizeof *clients * (nclients + 1));
		clients[nclients++].name = NULL;
		printf("Client %s has joined. There are now %d client(s).\n", ipbuf, (int)nclients);
		cu_net_sendmsg(client_rem, name_msg, sizeof name_msg);
	} else if (event_type == CUEVT_DISCONNECT) {
		if (!c->name) printf("Unnamed client %s has disconnected. There are now %d client(s).\n", ipbuf, (int)n);
		else {
			custr notify;
			custr_fmt(&notify, "[ Server ] %s has disconnected.", (char *)c->name);
			cu_server_broadcast(server, notify.str, notify.len + 1, NULL, 0);
			printf("%s has disconnected. There are now %d client(s).\n", (char *)c->name, (int)n);
			custr_clear(&notify);
			free(c->name);
		}
		--nclients;
	}
	return 1;
}

static void server_main(const char *port)
{
	cu_net_remote srem;
	if (!cu_server_listen(&srem, port, NULL, 0, 1024, server_event, -1)) printf("Could not start server.\n");
	free(clients);
}

int main(int argc, char **argv)
{
	char ipbuf[CU_NET_IPADDR_LEN];
	int ip_id = 0;

	if (argc < 2 || ((*argv[1] & 223) == 'C' && argc != 4) || ((*argv[1] & 223) == 'S' && argc != 3)) {
		fprintf(stderr, "Usage: %s [c]lient <address> <port> | [s]erver <port>\n", *argv);
		exit(EXIT_FAILURE);
	}

	while (cu_net_interfaces(ipbuf, CU_NET_INTERFACE_IPV4, ip_id++) && strncmp(ipbuf, "192.168.", 8));
	printf("IPv4: %s\n", ipbuf);

	if (!cu_net_init()) {
		fprintf(stderr, "Could not initialize networking library.");
		return 1;
	}

	if ((*argv[1] & 223) == 'C') client_main(argv[2], argv[3]);
	else server_main(argv[2]);
	cu_net_terminate();

	return 0;
}
