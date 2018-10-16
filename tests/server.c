/**
 * This file is part of netlib
 * which is licensed under the zlib license
 *
 * github.com/univrsal/netlib
 * netlib is a small network library
 * heavily based on SDL_net without
 * the dependency on SDL
 *
 * Documentation is directly taken from SDL_net
 * I take no credit for this code, I only
 * reformatted it for my needs and made slight changes
 * Source: libsdl.org/projects/SDL_net/docs/demos/tcpserver.c
 */

#include <stdlib.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <Windows.h>
#endif
#include <netlib.h>

int main(int argc, char **argv)
{
	ip_address ip, *remoteip;
	tcp_socket server, client;
	char message[1024];
	int len;
	uint32_t ipaddr;
	uint16_t port;

	/* check our commandline */
	if (argc<2)
	{
		printf("usage:\n [port]\n", argv[0]);
		exit(0);
	}

	/* initialize SDL_net */
	if (netlib_init() == -1)
	{
		printf("netlib_init: %s\n", netlib_get_error());
		exit(2);
	}

	/* get the port from the commandline */
	port = (uint16_t)strtol(argv[1], NULL, 0);

	/* Resolve the argument into an IPaddress type */
	if (netlib_resolve_host(&ip, NULL, port) == -1)
	{
		printf("netlib_resolve_host: %s\n", netlib_get_error());
		exit(3);
	}

	/* open the server socket */
	server = netlib_tcp_open(&ip);
	if (!server)
	{
		printf("netlib_tcp_open: %s\n", netlib_get_error());
		exit(4);
	}

	while (1)
	{
		/* try to accept a connection */
		client = netlib_tcp_accept(server);
		if (!client)
		{
			Sleep(100);
			continue;
		}

		/* get the clients IP and port number */
		remoteip = netlib_tcp_get_peer_address(client);
		if (!remoteip)
		{
			printf("netlib_tcp_get_peer_address: %s\n", netlib_get_error());
			continue;
		}

		/* print out the clients IP and port number */
		ipaddr = netlib_swap_BE32(remoteip->host);
		printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
			ipaddr >> 24,
			(ipaddr >> 16) & 0xff,
			(ipaddr >> 8) & 0xff,
			ipaddr & 0xff,
			remoteip->port);

		/* read the buffer from client */
		len = netlib_tcp_recv(client, message, 1024);
		netlib_tcp_close(client);
		if (!len)
		{
			printf("netlib_tcp_close: %s\n", netlib_get_error());
			continue;
		}

		/* print out the message */
		printf("Received: %.*s\n", len, message);

		if (message[0] == 'Q')
		{
			printf("Quitting on a Q received\n");
			break;
		}
	}

	/* shutdown SDL_net */
	netlib_quit();

	return 0;
}