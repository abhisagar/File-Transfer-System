#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> 

#define PORT 8080

void getFilename(int sock, char* filename)
{
	char recv_str[256]; 
	ssize_t rcvd_bytes; 

 	if ( (rcvd_bytes = recv(sock, recv_str, 256, 0)) < 0)
 	{
 		perror("recv error");
 		return;
 	}
	sscanf (recv_str, "%s\n", filename); 
}

int sendFile(int sock, char *filename)
{
 	int count; 
 	ssize_t read_bytes,sent_bytes,sent_file_size;
 	char buffer[256]; 
 	char * error_message = "File not found\n";
 	int f;
 	count = 0;
 	sent_file_size = 0;
 	
 	if( (f = open(filename, O_RDONLY)) < 0) 
 	{
		perror(filename);
 		if( (sent_bytes = send(sock, error_message ,strlen(error_message), 0)) < 0 )
 		{
 			perror("send error");
 			return -1;
 		}
 	}
	else 
 	{
 		printf("Sending file: %s\n", filename);
 		while( (read_bytes = read(f, buffer, 256)) > 0 )
 		{
 			if( (sent_bytes = send(sock, buffer, read_bytes, 0)) < read_bytes )
		 	{
			perror("send error");
		 	return -1;
		 	}
		 	count++;
		 	sent_file_size += sent_bytes;
		}
 		close(f);
 	} 
	printf("Done with this client. Sent %ld bytes in %d send(s)\n\n",sent_file_size, count);
	return count;
}

int main(int argc, char* argv[])
{
 	int socket_fd, sock;
 	struct sockaddr_in address, cli_addr;
 	socklen_t len;
 	char filename [256];
 	char print_addr [INET_ADDRSTRLEN]; 
 	memset(&address, 0, sizeof(address));
 	memset(&cli_addr, 0, sizeof(cli_addr));
 	address.sin_family = AF_INET;
 	address.sin_addr.s_addr = htonl(INADDR_ANY);
 	address.sin_port = (argc > 1) ? htons(atoi(argv[1])) : htons(PORT);
 
 	if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
 	{
 		perror("socket error");
 		exit(EXIT_FAILURE);
 	}
 	if(bind(socket_fd, (struct sockaddr*) &address, sizeof(address)) < 0 )
 	{
 		perror("bind error");
 		exit(EXIT_FAILURE);
 	}

 	printf("Listening on port number %d ...\n", ntohs(address.sin_port));
 	if( listen(socket_fd, 5) < 0 ) 
 	{
 		perror("listen error");
 		exit(EXIT_FAILURE);
 	}
 		len = sizeof(cli_addr);
 		printf ("Waiting for a client to connect...\n\n");
 		if ( (sock = accept(socket_fd, (struct sockaddr*) &cli_addr,&len)) < 0 )
	 	{
	 		perror("accept error");
	 		exit(EXIT_FAILURE); 
	 	}

 		inet_ntop(AF_INET, &(cli_addr.sin_addr), print_addr, INET_ADDRSTRLEN);
 		printf("Client connected from %s:%d\n",print_addr, ntohs(cli_addr.sin_port) );

 		getFilename(sock, filename);
 		sendFile(sock, filename);
 		printf("Closing connection\n");
 		close(sock);
 		 
 	close(socket_fd); 
 	return 0;
}
