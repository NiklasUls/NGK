/* A simple client in the internet domain using TCP
The ip adresse and port number on server is passed as arguments 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include "iknlib.h"

#define BUFSIZE 256

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{

	int sockfd, portno, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char bufferr[BUFSIZE];
    
	if(argc != 4)error("Mangler argument\n");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) error("socket");

	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if(hp == 0) error("Unknown host\n");

	bcopy((char*)hp->h_addr, (char*)&server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length = sizeof(struct sockaddr_in);
	n=sendto(sockfd, argv[3], strlen( argv[3]), 0, (const struct sockaddr *)&server, length);
	if(n<0) error("Sendto\n");
	n=recvfrom(sockfd, bufferr, BUFSIZE, 0, (struct sockaddr *)&from, &length);
	if(n<0) error("Recvfrom\n");
	printf(bufferr);


	close(sockfd);
	return 0;
}

