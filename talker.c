/*
** talker.c -- a datagram "client" demo
change talker.c to input function(search or prefix) word
get feedback from listener
20180405
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "21217"	// the port users will be connecting to 
							// Backend-Server (A)
#define IPADDRESS "127.0.0.1" // local IP address

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char recv_data[1024];
	struct sockaddr_storage their_addr;
	
	socklen_t addr_len;

	if (argc != 3) {
		fprintf(stderr,"usage: talker function word\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(IPADDRESS, SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
	// connect 2 strings
	char temp[64]="";
	strcat(temp, argv[1]);
	strcat(temp, argv[2]);
	if ((numbytes = sendto(sockfd, temp, strlen(temp), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	printf("talker: sent %d bytes to %s\n", numbytes, IPADDRESS);

	freeaddrinfo(servinfo);
	int bytes_recv = recvfrom(sockfd,recv_data,sizeof recv_data,0, NULL, NULL);
	if ( bytes_recv == -1) {
	    perror("recv");
	    exit(1);
	}
	printf("%d\n",bytes_recv );
   	recv_data[bytes_recv]= '\0';
   	printf("Received :%s\n",recv_data);

	close(sockfd);

	return 0;
}
