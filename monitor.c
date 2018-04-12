/*
** monitor.c 
** Yudi He
** ID: 5670857217
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "26217" // the TCP port of aws that monitor connect to

#define MAXDATASIZE 4000 // max number of bytes we can get at once 
#define IPADDRESS "127.0.0.1"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//get last index of a substring of recv_data begins at index startIndex
int getLastIndexOfStringSeg(char* recv_data, int startIndex){
	for (int i=startIndex; i<strlen(recv_data); i++)
	{
		if(recv_data[i]==':'&&recv_data[i+1]==':'&&recv_data[i+2]==':')
		{
			return i-1;
		}
	}
	return -1;
}

// get next substring from the buf
// currentIndex and lastIndexOfSubString will change automatically every time
char* getSubString(char *buf, int * currentIndex, int * lastIndexOfSubString ) 
{
	char* targetString = (char*) malloc(1000);
	memset(targetString,'\0',1000);
	(*lastIndexOfSubString) = getLastIndexOfStringSeg(buf, *currentIndex);
	strncpy(targetString, buf + (*currentIndex), (*lastIndexOfSubString) - (*currentIndex) + 1);
	*currentIndex = (*lastIndexOfSubString) + 4;
	return targetString;
}

int main(void)
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(IPADDRESS, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("monitor : socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("monitor : connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "monitor : failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("The monitor is up and running.\n");

	freeaddrinfo(servinfo); // all done with this structure
	
	while (1){
		// buf format: function:::<orther part>
		// if function is search: search:::difinition("0" if not found):::
		// onesimilarword("0 if not found"):::onesimilarword difinition

		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
		}
		printf("debug: receive from aws <%s>\n",buf);

		buf[numbytes] = '\0';
		while(numbytes != 0)
		{
			//printf("debug: numbytes=%d, monitor received %s\n",numbytes,buf);
			printf("debug: receive from aws <%s>\n",buf); 	// format: function:::<orther part>
															// if function is search: search:::difinition("0" if not found):::
															// onesimilarword("0 if not found"):::onesimilarword difinition
			numbytes=0;
			int currentIndex=0;
			int lastIndexOfSubString = getLastIndexOfStringSeg(buf, currentIndex);
		
			
			// get function name
			char *function = getSubString(buf,&currentIndex,&lastIndexOfSubString);

			// get word name
			char *word = getSubString(buf,&currentIndex,&lastIndexOfSubString);

			if(strcmp(function,"search")==0)
			{
				// printf("debug: function is search\n");
				// get definition name
				char *definition = getSubString(buf,&currentIndex,&lastIndexOfSubString);

				// printf("debug: definition is %s\n",definition);
				if (strcmp(definition,"0")==0) {
					printf("Didn't find a match for < %s >\n",word);
				} else {
					printf("Found a match for < %s >:\n< %s >\n",word, definition);
				}

				char * onesimilarword = getSubString(buf,&currentIndex,&lastIndexOfSubString);
				if (strcmp(onesimilarword,"0")==0) {
					printf ("Didn't find an edit distance match\n\n");
				} 
				else {
					char * oneSimilarWordDefinition = getSubString(buf,&currentIndex,&lastIndexOfSubString);
					printf("One edit distance match is <%s>:\n<%s>\n\n",onesimilarword,oneSimilarWordDefinition);
				}

			}
			else if(strcmp(function,"fix")==0){
				char *number = getSubString(buf,&currentIndex,&lastIndexOfSubString);
				char number2[10];
				strcpy(number2,number);
				if(strcmp(number,"0")==0){
					printf("\nFound no matches for <%s>\n",word);
				} else {
					int number=atoi(number2);
					printf("\nFound <%d> matches for <%s>:\n",number,word);
					while(number!=0){
						char *oneWord=getSubString(buf,&currentIndex,&lastIndexOfSubString);
						printf("<%s>\n", oneWord);
						number--;
					}
				}
			}
		}
		
		//exit(0);
	}
	close(sockfd);

	return 0;
}

