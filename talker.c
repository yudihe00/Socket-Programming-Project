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

/////////////////////////////////////////////////////////////////
// setupUDP and send function,word to specific port
// only used in udpQuery function
////////////////////////////////////////////////////////////////
int setupUDP(char *function, char *word, char* port)
{
	int sockfd;
	int rv;
	int numbytes;
	struct addrinfo hints, *servinfo, *p;
	socklen_t addr_len;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(IPADDRESS, port, &hints, &servinfo)) != 0) {
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
	// send function to server
	if ((numbytes = sendto(sockfd, function, strlen(function), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	// send word to server
	if ((numbytes = sendto(sockfd, word, strlen(word), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	printf("talker: sent <%s> and <%s> to %s\n", function, word, IPADDRESS);

	freeaddrinfo(servinfo); // done with servinfo
	return sockfd;
}

/////////////////////////////////////////////////////////////////
// udpQuery, based on function, word and port
////////////////////////////////////////////////////////////////
char* udpQuery(char *function, char *word, char* port)
{
	int sockfd;
	char * return_recv_data=(char *) malloc(100);
	char recv_data[1024];
	sockfd=setupUDP(function,word,port);
	int bytes_recv;
	
	bytes_recv = recvfrom(sockfd,recv_data,sizeof recv_data,0, NULL, NULL);
	if ( bytes_recv == -1) {
	    perror("recv");
	    exit(1);
	}
   	//recv_data[bytes_recv]= '\0';
   	printf("debug in udpQuery: Received :%s, bytes_recv is %d\n",recv_data,bytes_recv);
	close(sockfd);
	strcpy(return_recv_data,recv_data);
	return return_recv_data;
}

int main(int argc, char *argv[])
{
	int sockfd;	
	int numbytes;

	if (argc != 3) {
		fprintf(stderr,"usage: talker function word\n");
		exit(1);
	}
	char *recv_data=udpQuery(argv[1],argv[2],SERVERPORT);
	printf("Received :%s\n",recv_data);

	return 0;
}
