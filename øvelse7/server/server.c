/* A simple server in the internet domain using TCP
The port number is passed as an argument 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

#define BUFSIZE_RX 200
#define BUFSIZE_TX 1000

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	printf("Starting server...\n");

	int sockfd, newsockfd, portno;

	socklen_t clilen;
	char bufferRx[BUFSIZE_RX];
	char bufferTx[BUFSIZE_TX];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	
	if (argc < 2) 
		error("ERROR usage: port");


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	printf("Binding...\n");
	bzero(&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	printf("Listen...\n");
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr);


	while (1)
	{
		n = recvfrom(sockfd, bufferTx, BUFSIZE_TX, 0, (struct sockaddr *)&cli_addr, &clilen);
		if(n<0) error("recvfrom");
		write(1, "Received a datagram: ", 21);
		write(1, bufferTx, n);
		n = sendto(sockfd, "Got your message\n", 17, 0, (struct sockaddr *)&cli_addr, clilen);
		if(n<0) error("Sendto");
	}
	close(sockfd);
	return 0; 
}
