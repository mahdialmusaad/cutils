/*
   Example usage for provided networking functions from the cutils library.
   Main library code and documentation can found in cutils.c and cutils.h in the same directory.
   The cutils library is licensed under the GPLv2, see aforementioned files for full statement.
*/

#include "../cutils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if CU_OS_UNIX
#  define FDFMT "d"
#else
#  define FDFMT CU_UPTR_FMT
#endif

static int client_event(cu_net_remote *CU_RESTRICT server, enum cu_net_event event_type, void *CU_RESTRICT data, uptr n)
{
	CU_UNUSED(server);
	if (event_type == CUEVT_MESSAGE) {
		printf("%s\n", (char *)data);
		free(data);
	} else if (event_type == CUEVT_DISCONNECT && !n) printf("[ Client ] Disconnected by server.\n");
	else if (event_type == CUEVT_ALLOCDMEMERR) fprintf(stderr, "[ Client ] Failed to allocate memory for server's message.\n");
	else if (event_type == CUEVT_MSGLISTENERR) fprintf(stderr, "[ Client ] Generic event error: %s", cu_net_lasterr());
	else if (event_type == CUEVT_SIGNAL) {
		printf("[ Client ] Signal recieved, disconnecting.\n");
		return 0;
	} else if (event_type == CUEVT_HEARTBEAT) return 1;
	return 1;
}
static CU_THREAD_FUNCTION(client_scanf, server)
{
	while (1) {
		char msg[1024], *res = fgets(msg, 1024, stdin);
		if (res && strlen(msg) && msg[strlen(msg) - 1] == '\n') msg[strlen(msg) - 1] = '\0';
		if (!strlen(msg)) continue;
		if (!res || !strcmp(msg, "exit") || !strcmp(msg, "EXIT")) break;
		if (res && cu_net_sendmsg((cu_net_remote *)server, msg, strlen(msg) + 1) != CUERR_NONE) fprintf(stderr, "Failed to send message to server.");
	}
	cu_client_close((cu_net_remote *)server);
	return CU_THREAD_RETURN_VAL;
}

static void client_main(char *addr, long port)
{
	cu_net_remote server;
	enum cu_net_error err;
	cu_thread scanf_thr;

	if ((err = cu_client_start(&server, addr, (u16)port)) == CUERR_NONE) printf("Connected to %s (IP %s) on port %ld. You can input messages (max 1KiB) or EXIT.\n", addr, server.ip, port);
	else {
		if (err == CUERR_ARGS) fprintf(stderr, "Invalid port argument. See usage text.\n");
		else if (err == CUERR_ADDR) fprintf(stderr, "Could not get address info for '%s': %s\n", addr, cu_net_lasterr());
		else if (err == CUERR_CONNECT) fprintf(stderr, "Could not connect to address: %s\n", cu_net_lasterr());
		exit(EXIT_FAILURE);
	}

	if (!(scanf_thr = cu_thread_create(client_scanf, &server))) {
		fprintf(stderr, "Failed to create input thread.");
		goto fail;
	}

	cu_client_listen(&server, client_event, 1000);
fail:
	printf("Ending connection to '%s' (IP %s).\n", addr, server.ip);
	cu_client_close(&server);
}

static int named_clients;

static int server_event(cu_net_server *CU_RESTRICT server, cu_net_remote *CU_RESTRICT remote, enum cu_net_event event_type, void *CU_RESTRICT data, uptr n)
{
	if (event_type == CUEVT_MESSAGE) {
		uptr u, displayable = 0;
		char *strdata = (char *)data;

		if (!n && n > 1024) {
			printf("Kicking client %s (SID %" FDFMT ") for large message.\n", remote->ip, remote->fd);
			cu_server_disconnect_client(server, remote);
			return 1;
		}

		for (u = 0; u < n - 1; ++u) {
			if (strdata[u] && (strdata[u] <= ' ' || strdata[u] > '~')) strdata[u] = ' ';
			else ++displayable;
		}
		if (displayable < 1) return 1;

		if (!remote->ext) {
			custr notify = CUSTR_EMPTY;
			int i;

			if (n >= 31) strdata[30] = '\0';

			for (i = 1; i <= server->clients_count; ++i) {
				const char duplicate_msg[] = "[ Server ] That name is already in use.";
				if (!server->remotes[i].ext || strcmp((char *)server->remotes[i].ext, strdata)) continue;
				cu_net_sendmsg(remote, duplicate_msg, sizeof duplicate_msg);
				return 1;
			}

			printf("Client %s (SID %" FDFMT ") says their name is '%s'.\n", remote->ip, remote->fd, strdata);
			remote->ext = strdata;

			if (++named_clients == 1) custr_create(&notify, "[ Server ] Welcome, you are the only person connected.");
			else {
				custr_create(&notify, "[ Server ] Welcome, the other active user(s) are:");
				for (i = 1; i <= server->clients_count; ++i) {
					custr addfmt = CUSTR_EMPTY;
					if (server->remotes + i == remote || !server->remotes[i].ext) continue;
					custr_fmt(&addfmt, " '%s'", (char *)server->remotes[i].ext);
					custr_append(&notify, addfmt.str);
					custr_clear(&addfmt);
				}
			}

			cu_net_sendmsg(remote, notify.str, notify.len + 1);
			custr_clear(&notify);
			custr_fmt(&notify, "[ Server ] %s has joined.", strdata);
			cu_server_broadcast(server, notify.str, notify.len + 1, &remote, 1);
			custr_clear(&notify);
		} else {
			custr msg = CUSTR_EMPTY;
			custr_fmt(&msg, "%s: %s", (char *)remote->ext, strdata);
			printf("Broadcasting %s's %uB message to %d others: \"%.50s\"%s\n", (char *)remote->ext, (unsigned)n, server->clients_count - 1, strdata, n > 50 ? "..." : "");
			free(strdata);
			cu_server_broadcast(server, msg.str, msg.len + 1, &remote, 1);
			custr_clear(&msg);
		}
	} else if (event_type == CUEVT_CONNECT) {
		const char name_msg[] = "[ Server ] Enter a username to be identified by.";
		printf("Client %s (SID %" FDFMT ") has joined. There are now %d client(s).\n", remote->ip, remote->fd, (int)n);
		cu_net_sendmsg(remote, name_msg, sizeof name_msg);
	} else if (event_type == CUEVT_DISCONNECT) {
		if (!remote->ext) printf("Unnamed client %s (SID %" FDFMT ") has disconnected. There are now %d client(s).\n", remote->ip, remote->fd, (int)n);
		else {
			custr notify;
			custr_fmt(&notify, "[ Server ] %s has disconnected.", (char *)remote->ext);
			cu_server_broadcast(server, notify.str, notify.len + 1, NULL, 0);
			printf("%s has disconnected. There are now %d client(s).\n", (char *)remote->ext, (int)n);
			--named_clients;
			custr_clear(&notify);
			free(remote->ext);
		}
	} else if (event_type == CUEVT_ALLOCDMEMERR && remote) fprintf(stderr, "Failed to allocate memory for a message from client %s.\n", remote->ip);
	else if (event_type == CUEVT_ALLOCDMEMERR && !remote) fprintf(stderr, "OOM error.\n");
	else if (event_type == CUEVT_REMOTECONERR && n) fprintf(stderr, "Connection attempted, but server is full (%d client(s) connected).\n", (int)n);
	else if (event_type == CUEVT_REMOTECONERR) fprintf(stderr, "Connection accept error: %s\n", cu_net_lasterr());
	else if (event_type == CUEVT_MSGLISTENERR) fprintf(stderr, "Listen error: %s\n", cu_net_lasterr());
	else if (event_type == CUEVT_SIGNAL) {
		const char goodbye[] = "[ Server ] Server has been closed.";
		cu_server_broadcast(server, goodbye, sizeof goodbye, NULL, 0);
		return 0;
	} else if (event_type == CUEVT_HEARTBEAT) return 1;
	return 1;
}

static void server_main(long port, long maxclients)
{
	cu_net_server server;
	enum cu_net_error err = cu_server_start(&server, (u16)port, (int)maxclients);

	if (err == CUERR_NONE) printf("Started on port %ld, client limit %ld.\n", port, maxclients);
	else {
		if (err == CUERR_ARGS) fprintf(stderr, "Invalid argument(s). See usage text.\n");
		else if (err == CUERR_ADDR) fprintf(stderr, "Error when retrieving local address info: %s\n", cu_net_lasterr());
		else if (err == CUERR_MEM) fprintf(stderr, "OOM error.\n");
		else fprintf(stderr, "Generic error: %s", cu_net_lasterr());
		exit(EXIT_FAILURE);
	}

	cu_server_listen(&server, server_event, 20);
	printf("Closing server and %d connection(s).\n", server.clients_count);
	cu_server_close(&server);
}

int main(int argc, char **argv)
{
	char ipbuf[CU_NET_IPADDR_LEN], *ipres;
	int ip_id = 0;

	if (argc != 4 || (argc > 1 && (*argv[1] & 223) != 'C' && (*argv[1] & 223) != 'S')) {
		fprintf(stderr, "Usage: %s [c]lient <address> <port [1024,65535]> | [s]erver <port [1024,65535]> <maxclients [1+]>\n", *argv);
		exit(EXIT_FAILURE);
	}

	while ((ipres = cu_net_interfaces(ipbuf, CU_NET_INTERFACE_IPV4, ip_id++)) && strncmp(ipbuf, "192.168.", 8));
	printf("IPv4: %s\n", ipbuf);

	if (!cu_net_init()) {
		fprintf(stderr, "Could not initialize networking library.");
		exit(EXIT_FAILURE);
	}

	if ((*argv[1] & 223) == 'C') client_main(argv[2], strtol(argv[3], NULL, 0));
	else server_main(strtol(argv[2], NULL, 0), strtol(argv[3], NULL, 0));

	cu_net_terminate();
	return 0;
}
