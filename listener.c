/*
** listener.c -- a datagram sockets "server" demo
change to be used for the Backend-Server (A)
listen continuously
send feedback to talker.c
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

#define MYPORT "21217"	// the port users will be connecting to, server A

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	char function[MAXBUFLEN];
	char word[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	char send_data[1024];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	addr_len = sizeof their_addr;

	printf("listener: waiting to recvfrom...\n");

	while(1) {
		
		numbytes = recvfrom(sockfd, function, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len);

		function[numbytes] = '\0';

		numbytes = recvfrom(sockfd, word, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len);

		word[numbytes] = '\0';

		printf("\nlistener: got packet from %s\n",
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s));
		//printf("listener: packet is %d bytes long\n", numbytes);
		printf("listener: function is <%s>, word is <%s>\n ", function,word);
		strcpy(send_data,"test: search results");
    	printf(" SEND : %s\n",send_data);

    	numbytes = sendto(sockfd,send_data,strlen(send_data),0,
    		(struct sockaddr *)&their_addr, addr_len);
    	printf("debug: numbytes is %d\n", numbytes);
    	if (numbytes==-1) {
    		perror("recv");
	    	exit(1);
    	}
    	fflush(stdout); //wait for next connect

		//close(sockfd);
	}

	return 0;
}
