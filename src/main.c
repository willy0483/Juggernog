#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <openssl/ssl.h>

int main()
{

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(443),
		.sin_addr = { .s_addr = htonl(0x08080808) },
	};

	int connection_state = connect(socketfd, (struct sockaddr*)&addr, sizeof(addr));
	if(connection_state == -1)
	{
		printf("There ware an error makeing a connection to socket\n\n");
	}

	SSL_CTX* ctx = SSL_CTX_new(TLS_method());
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, socketfd);
	SSL_connect(ssl);

	char* request = "GET /\r\n\r\n";
	SSL_write(ssl, request, strlen(request));

	char buffer[1024] = { 0 };
	SSL_read(ssl, buffer, 1023);

	printf("Response: \n %s \n", buffer);

	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	close(socketfd);
	return 0;
}
