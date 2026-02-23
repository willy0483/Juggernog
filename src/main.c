#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <quickrevive/quickrevive.h>

#define PORT "80"

void* get_in_addr(struct sockaddr* sa);

int main(int argc, char* argv[])
{
	int socketfd;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* p;

	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int addr_info_state = getaddrinfo("www.example.com", PORT, &hints, &res);
	if(addr_info_state != 0)
	{
		qlog(QLOG_ERROR, "addr", "getaddrinfo: %s", gai_strerror(addr_info_state));
		return 1;
	}

	for(p = res; p != NULL; p = p->ai_next)
	{
		if((socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			qlog(QLOG_ERROR, "socketfd", "failed to create socket");
			continue;
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
		qlog(QLOG_INFO, "connect", "attempting connection to %s", s);

		if(connect(socketfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			qlog(QLOG_ERROR, "connect", "failed to connect to remote socket\n");
			continue;
		}
		break;
	}

	if(p == NULL)
	{
		qlog(QLOG_ERROR, "connect", "failed to connect");
		return 1;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
	qlog(QLOG_SUCCESS, "connect", "connected to %s", s);

	freeaddrinfo(res);

	close(socketfd);
	return 0;
}

void* get_in_addr(struct sockaddr* sa)
{
	if(sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
