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
	uint8_t bufferRx[BUFSIZE_RX];
	uint8_t bufferTx[BUFSIZE_TX];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	
	if (argc < 2) 
		error("ERROR usage: port");


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	printf("Binding...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	printf("Listen...\n");
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr);

	for (;;)
	{
		char recievedMessage[BUFSIZE_RX];
		char sendMessage[BUFSIZE_TX];
		const char* filename;

		printf("Accept...\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) error("ERROR on accept");
		else printf("Accepted\n");


		readTextTCP(newsockfd, recievedMessage, BUFSIZE_RX);
		printf("Read done \n");
		filename = extractFileName(recievedMessage);
		printf("Client ask for filename: %s\n", filename);

		//Filehandling
		size_t numBytes;
		FILE * fp;
		fp = fopen(filename, "rb");	
		if(fp == 0){
			writeTextTCP(newsockfd, "-1");
			printf("Requested file doesn't exists: %s\n", filename);
		} else {
			
			long fileSize = getFilesize(filename);
			snprintf(sendMessage, BUFSIZE_TX, "%ld", fileSize);
			writeTextTCP(newsockfd, sendMessage);	//Send filesize to client
			printf("Filesize is send\n");

			char tmpBuf[BUFSIZE_TX];
			numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
			int total_bytes=0;
			while (numBytes) {
				total_bytes = total_bytes + write(newsockfd, tmpBuf, numBytes);
				printf("Send bytes: %i of %ld\n", total_bytes, fileSize);
				numBytes = fread(tmpBuf, 1, sizeof(tmpBuf), fp);  // Automatic seek!
			}
			
			fclose(fp);
		}
		
		close(newsockfd);
	}
	close(sockfd);
	return 0; 
}
