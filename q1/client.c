#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> 
#define PORT 8080


int recv_file(int sock, char* file_name)
{
 	char send_str [256];
 	int f; 
 	ssize_t sent_bytes, bytes, filesize;
 	char * error_message = "File not found\n";
 	int count; 
 	char buffer[256]; 
 	size_t send_strlen;

 	sprintf(send_str, "%s\n", file_name);
 	send_strlen = strlen(send_str);
 	if( (sent_bytes = send(sock, file_name, send_strlen, 0)) < 0 ) 
 	{
 		perror("send error");
 		return -1;
 	}
 	bytes = recv(sock,buffer,256,0);
 	if(strcmp(error_message,buffer)==0)
 	{
 		printf("%s\n",error_message);
 		return -1;
 	}
 	else
 	{
 		if ( (f = open(file_name, O_WRONLY|O_CREAT, 0644)) < 0 )
	 	{
	 		perror("error creating file");
	 		return -1;
	 	}
		count++;
 		filesize += bytes;

	 	if (write(f, buffer, bytes) < 0 )
	 	{
	 		perror("error writing to file");
	 		return -1;
	 	} 		
 	}
 	

 	count = 0;
	filesize = 0; 
 	while ( (bytes = recv(sock, buffer, 256, 0)) > 0 )
 	{

 		count++;
 		filesize += bytes;

	 	if (write(f, buffer, bytes) < 0 )
	 	{
	 		perror("error writing to file");
	 		return -1;
	 	}
 	}
	close(f); 
 	printf("Client Received: %ld bytes in %d recv(s)\n", filesize,count);
 	return filesize;
}


int main(int argc, char* argv[])
{
 	int sock_fd;
 	struct sockaddr_in srv_addr;

 	if (argc < 3)
 	{
 		printf("usage: %s <filename> <IP address> [port number(default 8080)] \n", argv[0]);exit(EXIT_FAILURE);
 	}
 	memset(&srv_addr, 0, sizeof(srv_addr)); 
	sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 	srv_addr.sin_family = AF_INET; 
 	if ( inet_pton(AF_INET, argv[2], &(srv_addr.sin_addr)) < 1 )
 	{
 		printf("Invalid IP address\n");
		exit(EXIT_FAILURE);
 	}

 	srv_addr.sin_port = (argc > 3) ? htons(atoi(argv[3])) : htons(PORT);

 	if( connect(sock_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0 )
 	{
 		perror("connect error");
 		exit(EXIT_FAILURE);
 	}
 	printf("connected to:%s:%d ..\n",argv[2],PORT);

 	recv_file(sock_fd, argv[1]); 

 	if(close(sock_fd) < 0)
 	{
 		perror("socket close error");
 		exit(EXIT_FAILURE);
 	}
 	return 0;
}