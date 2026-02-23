#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <quickrevive/quickrevive.h>

#define PORT "3490"

int main(int argc, char* argv[])
{
	int socketfd;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* p;

	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int addr_info_state = getaddrinfo("www.example.net", PORT, &hints, &res);
	if(addr_info_state != 0)
	{
		qlog(QLOG_ERROR, "addr", "getaddrinfo: %s", gai_strerror(addr_info_state));
		return 1;
	}

	for(p = res; p != NULL; p = p->ai_next)
	{
		void* addr;
		char* ipver;

		struct sockaddr_in* ipv4;
		struct sockaddr_in6* ipv6;

		if(p->ai_addr->sa_family == AF_INET)
		{
			ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else
		{
			ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		qlog(QLOG_SUCCESS, "IP", "  %s: %s", ipver, ipstr);
	}

	freeaddrinfo(res);
	return 0;
}
