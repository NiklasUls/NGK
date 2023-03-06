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

#define BUFSIZE 1000

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	uint8_t buffer[BUFSIZE];
	FILE * fp;
	ssize_t numBytes;
	char bufferr[BUFSIZE];
	int bytes_written = 0;
    
	if (argc < 3)
	    error( "ERROR usage: ""hostname"",  ""port""");

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	    error("ERROR opening socket");

	server = gethostbyname(argv[1]);
	if (server == NULL) 
	    error("ERROR no such host");

	printf("Server at: %s, port: %s\n",argv[1], argv[2]);

	printf("Connect...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	    error("ERROR connecting");

	printf("Please enter the message: ");
	char input[256];
	fgets(input, sizeof(input),stdin);
	input[strcspn(input, "\n")]=0;
	n = write(sockfd, input, sizeof(input));
	if(n == 0) error("Failed to send filename to server\n");
	
	const char* fileName = extractFileName(input);	//Extract filename from user-input.
	long fileSize = readFileSizeTCP(sockfd);	//Read filesize wich is send from server
	if (fileSize == -1) 
	    error("File doesnt exist");
	printf("\nFilename: %s, Filesize: %ld bytes\n", fileName, fileSize);


	
	fp = fopen(fileName, "wb");
	int bytes_to_recieve=0;
	int i=1;
	while (1)
	{
		bytes_to_recieve = ((BUFSIZE*i++>fileSize)? (fileSize%BUFSIZE) : BUFSIZE);
		numBytes = read(sockfd, bufferr, bytes_to_recieve);
		printf("Bytes to recieve: %i. numBytes: %zu\n", bytes_to_recieve, numBytes);
		fwrite(bufferr, 1, numBytes, fp);
		bzero(bufferr, BUFSIZE);
		if(bytes_to_recieve != BUFSIZE) break;
	}
	
	fclose(fp);

    printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}

