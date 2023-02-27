/* A simple client in the internet domain using TCP
The ip adresse and port number on server is passed as arguments 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 
Modified: Michael Alrøe
Extended to support file client!
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include "iknlib.h"

#define STRBUFSIZE 256
#define DATASIZE 1000

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
 * @brief Receives a file from a server socket
 * @param serverSocket Socket stream to server
 * @param fileName Name of file. Might include path on server!
 */

void receiveFile(int serverSocket, const char* fileName, long fileSize)
{
	printf("Receiving: '%s', size: %li\n", fileName, fileSize);

}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[STRBUFSIZE];
	uint8_t dataBuf[DATASIZE];
	size_t fileSize;
	FILE * fp;
    
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

	printf("Please enter file name: ");
	fgets((char*)buffer,sizeof(buffer),stdin);
	buffer[strcspn(buffer,"\n")]=0;
	writeTextTCP(sockfd, buffer);

	const char * fileName = extractFileName(buffer);
	fileSize = readFileSizeTCP(sockfd);
	if (fileSize == -1)
	{
		error("File doesnt exist\n");
	}
	printf("Filename: %s, Filesize, %ld bytes\n", fileName, fileSize);
	

	
	fp = fopen(fileName, "wb");

	int bytesToRecieve = 0;
	int bytesRecieved = 0;
	float numBytes = 0;

	printf("Downloading file....\n");
	int i = 0;
	for(;;)
	{
		bytesToRecieve = ((fileSize - bytesRecieved) > 1000 ?  1000 : (fileSize-bytesRecieved) );
		numBytes = recv(sockfd, dataBuf, bytesToRecieve, MSG_WAITALL);

		bytesRecieved += numBytes;
		printf("Bytes recieved: %i of %ld\n", bytesRecieved, fileSize);
		
		fwrite(dataBuf, 1, numBytes, fp);
		bzero(dataBuf, DATASIZE);
		if(bytesToRecieve < DATASIZE) break;		
	}
	fclose(fp);
	printf("\n\nExprected filesize: %ld\n", fileSize);
	printf("Actual filesize: %ld\n", getFilesize(fileName));
	printf("Bytes missing: %ld\n", fileSize - getFilesize(fileName));
	
	
	printf("Closing Client\n");


	return 0;
}