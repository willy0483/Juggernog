#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <quickrevive/quickrevive.h>

#define PORT "80"

// https://beej.us/guide/bgnet/html/split/system-calls-or-bust.html#system-calls-or-bust
// 5.6 accept()—“Thank you for calling port 3490.”

void* get_in_addr(struct sockaddr* sa);

int main(int argc, char* argv[])
{
	int socketfd;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* p;

	char s[INET6_ADDRSTRLEN];

	int bytes;
	int sent;
	int received;
	int total;

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

	char* request = "GET / HTTP/1.1\r\n"
					"Host: www.example.com\r\n"
					"Connection: close\r\n"
					"\r\n";

	total = strlen(request);
	sent = 0;
	while(sent < total)
	{
		bytes = send(socketfd, request + sent, total - sent, 0);
		if(bytes < 0)
		{
			qlog(QLOG_ERROR, "send", "Failed sent message to socket");
		}
		if(bytes == 0)
		{
			break;
		}
		sent += bytes;
	}

	char response[4096];
	total = sizeof(response) - 1;
	received = 0;

	while(received < total)
	{
		bytes = recv(socketfd, response + received, total - received, 0);
		if(bytes < 0)
		{
			qlog(QLOG_ERROR, "recv", "Failed read message from socket");
		}
		if(bytes == 0)
		{
			break;
		}
		received += bytes;
	}

	if(received == total)
	{
		qlog(QLOG_ERROR, "storing", "storing complete response from socket");
	}

	qlog(QLOG_INFO, "Response", "%s", response);

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
