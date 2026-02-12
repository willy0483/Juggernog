#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#define PORT "9002"

int main()
{
	int socketfd;
	struct addrinfo hints;
	struct addrinfo* servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int addr_info_state = getaddrinfo(NULL, PORT, &hints, &servinfo);
	if(addr_info_state != 0)
	{
		printf("getaddrinfo: %s\n", gai_strerror(addr_info_state));
		return 1;
	}

	socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if(socketfd == -1)
	{
		printf("Failed to create socket: %s\n", gai_strerror(socketfd));
		return 1;
	}

	if(connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		printf("There ware an error makeing a connection to remote socket\n\n");
		return 1;
	}

	char server_response[256];
	if(recv(socketfd, &server_response, sizeof(server_response), 0) == -1)
	{
		printf("Failed to read from server\n");
		return 1;
	}

	// print out the servers response
	printf("The server sent data: %s\n", server_response);

	close(socketfd);
	return 0;
}
