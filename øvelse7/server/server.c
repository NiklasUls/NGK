	/* Creates a datagram server.  The port 
	number is passed as an argument.  This
	server runs forever */

	#include <sys/types.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <string.h>
	#include <netdb.h>
	#include <stdio.h>
	#include "iknlib.h"

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sock, length, n, fejl;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buf_rx[1];
	char buf_tx[1024];

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}
	
	sock=socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) error("Opening socket");

	length = sizeof(server);
	bzero(&server,length);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(atoi(argv[1]));
	if (bind(sock,(struct sockaddr *)&server,length)<0) 
		error("binding");
	fromlen = sizeof(struct sockaddr_in);
	while (1) {
		n = recvfrom(sock,buf_rx,1 ,0,(struct sockaddr *)&from,&fromlen);
		if (n < 0) error("recvfrom");
		write(1,"Received a datagram: ",21);
		write(1,buf_rx,n);
		size_t numBytes;
		FILE * fp;
		const char* filename;
		if(buf_rx[0]=='U' || buf_rx[0]=='u'){
			filename = "/proc/uptime";
		} else if(buf_rx[0]=='L' || buf_rx[0]=='l'){
			filename = "/proc/loadavg";
		} else {
			fejl=1;
		}
		fp=fopen(filename, "rb");
		if(fp == 0 || fejl != 0){
			fejl = 0;
			n = sendto(sock,"Wrong input\n",12, 0,(struct sockaddr *)&from,fromlen);
			printf("Input ikke korrekt\n");
			if (n  < 0) error("sendto");
		} else{
			long fileSize = getFilesize(filename);
			numBytes = fread(buf_tx, 1, sizeof(buf_tx), fp);
			n = sendto(sock,buf_tx, numBytes, 0,(struct sockaddr *)&from, fromlen);
			if(n<0) error("Unable to send data from file\n");		
		}
		
	}
	return 0;
}

