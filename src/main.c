#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main()
{

	// create socket
	int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(socketfd == -1)
	{
		printf("Failed to create socket\n");
		return 1;
	}

	// specify an address for the socket
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9002),
		.sin_addr = { .s_addr = INADDR_ANY },
	};

	int connection_state = connect(socketfd, (struct sockaddr*)&addr, sizeof(addr));
	if(connection_state == -1)
	{
		printf("There ware an error makeing a connection to remote socket\n\n");
		return 1;
	}

	char server_response[256];
	int read_state = recv(socketfd, &server_response, sizeof(server_response), 0);
	if(read_state == -1)
	{
		printf("Failed to read from server\n");
		return 1;
	}

	// print out the servers response
	printf("The server sent data: %s\n", server_response);

	close(socketfd);
	return 0;
}
