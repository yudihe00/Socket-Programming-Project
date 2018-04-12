/*
** aws.c
** Yudi He
** ID: 5670857217
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORTCLIENT "25217"  // the port for TCP with client
#define PORTMONITOR "26217"  // the port for TCP with monitor
#define SERVERPORTA "21217"	// the port users will be connecting to 
							// Backend-Server (A)
#define SERVERPORTB "22217"	// the port users will be connecting to 
							// Backend-Server (B)
#define SERVERPORTC "23217"	// the port users will be connecting to 
							// Backend-Server (C)
#define IPADDRESS "127.0.0.1" // local IP address

#define BACKLOG 10	 // how many pending connections queue will hold

// used in clear_dead_process()
void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// use sigaction to clear dead process
void clear_dead_process()
{
	struct sigaction sa; //sigaction() code is responsible for reaping zombie processes 
						//that appear as the fork()ed child processes exit.
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
}

// setup TCP at port
int setupTCP(char* port)
{
	int rv; // use for error checking of getaddrinfo()
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	memset(&hints, 0, sizeof hints); //Zero the whole structure before use
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM; //TCP SOCK_STREAM sockets
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
		// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	clear_dead_process();
	return sockfd;
}

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
	if (strcmp(port,SERVERPORTA)==0) {
		printf("Sent <%s> and <%s> to Backend-Server A\n", function, word);
	} else if (strcmp(port,SERVERPORTB)==0) {
		printf("Sent <%s> and <%s> to Backend-Server B\n", function, word);
	} else printf("Sent <%s> and <%s> to Backend-Server C\n", function, word);
	
	//printf("talker: sent <%s> and <%s> to %s\n", function, word, IPADDRESS);

	freeaddrinfo(servinfo); // done with servinfo
	return sockfd;
}

/////////////////////////////////////////////////////////////////
// udpQuery, according to function, word and port
////////////////////////////////////////////////////////////////
char* udpQuery(char *function, char *word, char* port)
{
	int sockfd;
	char * return_recv_data=(char *) malloc(4000); // return to main
	memset(return_recv_data,'\0',100);
	char recv_data[4000]=""; // save the udp return result
	sockfd=setupUDP(function,word,port);
	int bytes_recv;
	
	bytes_recv = recvfrom(sockfd,recv_data,sizeof recv_data,0, NULL, NULL);
	if ( bytes_recv == -1) {
	    perror("recv");
	    exit(1);
	}
   	recv_data[bytes_recv]= '\0';
   	//printf("debug in udpQuery: Received :%s, bytes_recv is %d\n",recv_data,bytes_recv);
	close(sockfd);
	strcpy(return_recv_data,recv_data);
	return return_recv_data;
}

// send text[] to monitor
int send_to_monitor(char text[], int new_fd_monitor)
{	
	char text2[4000];
	strcpy (text2,text);
	if (send(new_fd_monitor, text, strlen(text2), 0) == -1)
	{
		perror("send");
		return -1;
	}
	return 0;
}

//get last index of a substring of recv_data begins at index startIndex
int lastIndexOfStringSeg(char* recv_data, int startIndex){
	for (int i=startIndex; i<strlen(recv_data); i++)
	{
		if(recv_data[i]==':'&&recv_data[i+1]==':'&&recv_data[i+2]==':')
		{
			return i-1;
		}
	}
	return -1;
}

// Struct searchResult
// save search result from one server
struct searchResult {
	int matchNumber;
	char definition[101];
	int similarNumber;
	char oneSimilarWord[100];
	char oneSimilarWordDifinition[101];
};


// Process similar word part of the recv_data search funciton
// Return the struct searchResult with similar relavant variable
struct searchResult similarResultInOneSearch(int currentIndex, struct searchResult returnResult,char *recv_data)
{	
	int lastIndexOfSubstring;
	char tempString[2000]="";
	lastIndexOfSubstring=lastIndexOfStringSeg(recv_data,currentIndex);
	strncpy(tempString,recv_data+currentIndex,lastIndexOfSubstring - currentIndex+1);
	
	// convert the number into int format
	// returnResult.similarNumber=stringToInt(tempString);
	if (strcmp(tempString,"1")==0) {
		returnResult.similarNumber=1;
	} else returnResult.similarNumber=0;
	// printf("debug: number of similar word, tempString is %s, number is %d\n",tempString,returnResult.similarNumber);
	
	if(returnResult.similarNumber==0) return returnResult; // No similar word
	else { // have similar words
		
		// get one similar word
		currentIndex=lastIndexOfSubstring+4;
		lastIndexOfSubstring=lastIndexOfStringSeg(recv_data,currentIndex);
		strncpy(returnResult.oneSimilarWord,recv_data+currentIndex,lastIndexOfSubstring - currentIndex+1);
		
		// get the difinition of the similar word
		currentIndex=lastIndexOfSubstring+4;
		lastIndexOfSubstring=lastIndexOfStringSeg(recv_data,currentIndex);
		strncpy(returnResult.oneSimilarWordDifinition,recv_data+currentIndex,lastIndexOfSubstring - currentIndex+1);
		

	}
	return returnResult;
}

// Perfom one search, return searchResult
struct searchResult oneSearch(char *recv_data)
{	
	int currentIndex=0;
	int lastIndexOfSubstring;
	char tempString[2000]="";
	struct searchResult returnResult;
	memset(&returnResult, 0, sizeof returnResult); //initialize
	if (recv_data[0]=='0'){ // If no match word
		returnResult.matchNumber=0;

		// get similar word number
		currentIndex+=4; //startIndex of number of similar word
		returnResult = similarResultInOneSearch(currentIndex, returnResult,recv_data);
		
	} else { //have match words
		returnResult.matchNumber=1;
		// get one definition of that word
		lastIndexOfSubstring=lastIndexOfStringSeg(recv_data,currentIndex);
		strncpy(returnResult.definition,recv_data+currentIndex,lastIndexOfSubstring - currentIndex+1);
		// printf("debug: definition is <%s>\n", returnResult.definition);

		// get similar word number 
		currentIndex = lastIndexOfSubstring+4;
		returnResult = similarResultInOneSearch(currentIndex, returnResult,recv_data);

	}
	return returnResult;
}

// send search result to client and monitor
void sendSearchResult(char* word, char* recv_dataA, char* recv_dataB, char* recv_dataC,  int new_fd, int new_fd_monitor)
{
	char matchNumberString[10]="0"; //number of same words
	char similarNumberString[10]="0";
	char definition[101]=""; //definition of match word
	char oneSimilarWord[100]="0";
	char oneSimilarWordDifinition[101]=""; //difinition of one of the similar word
	// char similarWordString[4000];

	// returnString[300]: the result sendback to client and monitor
	// format: search:::word:::difinition("0" if not found):::
	// onesimilarword("0 if not found",end):::onesimilarword difinition
	char returnString[300]="search:::"; 
	strcat(returnString,word);
	strcat(returnString,":::");

	struct searchResult resultA;
	struct searchResult resultB;
	struct searchResult resultC;
	resultA=oneSearch(recv_dataA);
	resultB=oneSearch(recv_dataB);
	resultC=oneSearch(recv_dataC);
	printf("The AWS received <%d> similar words from Backend-Server <A> using UDP over port <21217>\n",resultA.similarNumber);
	printf("The AWS received <%d> similar words from Backend-Server <B> using UDP over port <22217>\n",resultB.similarNumber);
	printf("The AWS received <%d> similar words from Backend-Server <C> using UDP over port <23217>\n",resultB.similarNumber);

	// printf("debug resultA matchNumber is %d\n", resultA.matchNumber);
	// printf("debug: resultA definition is %s\n",resultA.definition);
	if (resultA.matchNumber!=0) { // Only save one definition
		strcpy(definition,resultA.definition);
		strcpy(matchNumberString,"1");
	} 
	else if (resultB.matchNumber!=0) {
		strcpy(definition,resultB.definition);
		strcpy(matchNumberString,"1");
	} else if (resultC.matchNumber!=0) {
		strcpy(definition,resultC.definition);
		strcpy(matchNumberString,"1");
	} else {strcpy(definition,"0"); strcpy(matchNumberString,"0");}

	if (resultA.similarNumber!=0) { 
		strcpy(oneSimilarWord,resultA.oneSimilarWord);
		strcpy(oneSimilarWordDifinition,resultA.oneSimilarWordDifinition);
		strcpy(similarNumberString,"1");
	} else if (resultB.similarNumber!=0) {
		strcpy(oneSimilarWord,resultB.oneSimilarWord);
		strcpy(oneSimilarWordDifinition,resultB.oneSimilarWordDifinition);
		strcpy(similarNumberString,"1");
	} else if (resultC.similarNumber!=0) {
		strcpy(oneSimilarWord,resultC.oneSimilarWord);
		strcpy(oneSimilarWordDifinition,resultC.oneSimilarWordDifinition);
		strcpy(similarNumberString,"1");
	} 

	//return String
	//format: definition("0", if no match):::similar word("0",if not fine):::definition:::
	strcat(returnString,definition);
	strcat(returnString,":::");
	// printf("debug: the return string is %s\n", returnString);
	strcat(returnString,oneSimilarWord);
	strcat(returnString,":::");
	if(strcmp(oneSimilarWord,"0")!=0) {
		strcat(returnString,oneSimilarWordDifinition);
		strcat(returnString,":::");
	}
	// printf("debug: the return string is <%s>\n", returnString);
	//return returnString;

	// send search result back to client
	if (send(new_fd, returnString, strlen(returnString), 0) == -1)
		perror("send");
	
	else printf("The AWS sent <%s> matches to client\n", matchNumberString);
	

	// send to monitor
	// char test3[1000]="aws send to monitor !";
	if(send_to_monitor(returnString,new_fd_monitor)!=-1){
		
		if (strcmp(oneSimilarWord,"0")==0 ){ //didn't find similar word, only send word to monitor
			printf("The AWS sent <%s> to client to the monitor via TCP port <26217>\n", word);
		}
		else printf("The AWS sent <%s> and <%s> to client to the monitor via TCP port <26217>\n", word,oneSimilarWord);
	}
	
	// printf("debug: test for B, matchNumber for non exist word is %d\n",resultB.matchNumber);
	// printf("debug: test for B, one similar word is <%s>, definition is <%s>\n",resultB.oneSimilarWord, resultB.oneSimilarWordDifinition);

}

void sendPrefixOrSuffixResult(char* word, char* recv_dataA, char* recv_dataB, char* recv_dataC, int new_fd, int new_fd_monitor)
{
	int currentIndex=0;
	int lastIndexOfSubstring=lastIndexOfStringSeg(recv_dataA,currentIndex);
	char tempString[10]="";
	char wordsString[5000]="";
	char combinedWordsString[5000]="";
	char returnString[5000]="fix:::";
	strcat(returnString,word);
	strcat(returnString,":::");
	int number = 0; //temp place to save a number of words from one server
	int numberTotal = 0; // total number of words
	
	// get number of words in recv_dataA
	strncpy(tempString,recv_dataA+currentIndex,lastIndexOfSubstring - currentIndex+1);
	currentIndex = lastIndexOfSubstring + 4;
	number = atoi(tempString);
	printf("The AWS received <%d> matches from Backend-Server <A> using UDP over port <21217>\n", number);
	// printf("debug: numberA is %d\n", number);
	numberTotal += number;

	if (number!=0){
		strncpy(wordsString,recv_dataA+currentIndex,strlen(recv_dataA) - currentIndex);
		strcat(combinedWordsString,wordsString);
		// printf("debug: wordsString is %s\n", wordsString);
	}

	// get number of words in recv_dataB
	memset(wordsString,'\0',strlen(wordsString));
	memset(tempString,'\0',strlen(tempString));
	currentIndex=0;
	lastIndexOfSubstring=lastIndexOfStringSeg(recv_dataB,currentIndex);
	strncpy(tempString,recv_dataB+currentIndex,lastIndexOfSubstring - currentIndex+1);
	currentIndex = lastIndexOfSubstring + 4;
	number = atoi(tempString);
	// printf("debug: numberB is %d\n", number);
	printf("The AWS received <%d> matches from Backend-Server <B> using UDP over port <22217>\n", number);
	numberTotal += number;
	if (number!=0){
		strncpy(wordsString,recv_dataB+currentIndex,strlen(recv_dataB) - currentIndex);
		strcat(combinedWordsString,wordsString);
		// printf("debug: wordsString is %s\n", wordsString);
	}

	// get number of words in recv_dataC
	memset(wordsString,'\0',strlen(wordsString));
	memset(tempString,'\0',strlen(tempString));
	currentIndex=0;
	lastIndexOfSubstring=lastIndexOfStringSeg(recv_dataC,currentIndex);
	strncpy(tempString,recv_dataC+currentIndex,lastIndexOfSubstring - currentIndex+1);
	currentIndex = lastIndexOfSubstring + 4;
	number = atoi(tempString);
	printf("The AWS received <%d> matches from Backend-Server <C> using UDP over port <23217>\n", number);
	// printf("debug: numberC is %d\n", number);
	numberTotal += number;
	if (number!=0){
		strncpy(wordsString,recv_dataC+currentIndex,strlen(recv_dataC) - currentIndex);
		// printf("debug: wordsString is %s\n", wordsString);
		strcat(combinedWordsString,wordsString);
		// printf("debug: number total is %d\n", numberTotal);
		// printf("debug: final wordsString is %s\n", combinedWordsString);
	}
	char numString[20];
	sprintf(numString, "%d", numberTotal);
	strcat(returnString,numString);
	strcat(returnString,":::");
	strcat(returnString,combinedWordsString);
	// printf("debug: return string is %s\n", returnString);

	// send search result back to client
	if (send(new_fd, returnString, strlen(returnString), 0) == -1)
		perror("send");
	
	else printf("The AWS sent <%d> matches to client\n", numberTotal);
	

	// send to monitor
	// char test3[1000]="aws send to monitor !";
	send_to_monitor(returnString,new_fd_monitor);
	printf("The AWS sent <%d> matches to the monitor via TCP port <26217>\n", numberTotal);
		

}

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	int sockfd_monitor,new_fd_monitor; //listen on sock_fd_monitor, new connection on new_fd_monitor
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size; // used for accept()
	char s[INET6_ADDRSTRLEN];
	char buf[100]; // temp saving place for receiving
	char function[6]; //function sent by client
	char word[100]; //word sent by client
	int numbytes; //using in receive or send

	sockfd = setupTCP(PORTCLIENT);
	sockfd_monitor = setupTCP(PORTMONITOR);
	//printf("debug: sockfd is %d\n", sockfd);
	//printf("debug: sockfd_monitor is %d\n", sockfd_monitor);
	printf("The AWS is up and running.\n");

	int monitorOn=0;
	while (monitorOn == 0){
		// child socket connect with monitor
		sin_size = sizeof their_addr;
		new_fd_monitor = accept(sockfd_monitor, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd_monitor == -1) {
			perror("accept");
			continue;
		}
		//printf("debug:monitorOn=%d\n",monitorOn);
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		//printf("server: got connection from %s\n", s);
		if (!fork()) { // this is the child process
			close(sockfd_monitor); // child doesn't need the listener
			monitorOn=1;
			//printf("debug:monitorOn=%d\n",monitorOn);
			continue;		
		}
		close(new_fd_monitor);  // parent doesn't need this
	}
	//printf("debug:out monitorOn=%d\n",monitorOn);

	while(1) 
	{  	// main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		//printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			
			// receive function and word from client
			if ((numbytes = recv(new_fd, function, sizeof function, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			function[numbytes] = '\0';
			// printf("debug: function is %s\n", function);
			//strcpy(function,buf);
			if ((numbytes = recv(new_fd, word, sizeof word, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			word[numbytes] = '\0';
			// printf("debug: word is %s\n", word );
			
			printf("The AWS received input=<%s> and function=<%s> from the client using TCP over port 25217\n",word,function);

			char recv_dataA[4000]="";
			char recv_dataB[4000]="";
			char recv_dataC[4000]="";
			//test for udp
			char *recvTemp = udpQuery(function,word,SERVERPORTA);
			strcpy(recv_dataA,recvTemp);
			free(recvTemp);
			recvTemp = NULL;
			// printf("debug: Received from serverA: %s\n",recv_dataA);

			//test for udp
			recvTemp=udpQuery(function,word,SERVERPORTB);
			strcpy(recv_dataB,recvTemp);
			free(recvTemp);
			recvTemp = NULL;
			// printf("debug: Received from serverB: %s\n",recv_dataB);

			//test for udp
			recvTemp=udpQuery(function,word,SERVERPORTC);
			strcpy(recv_dataC,recvTemp);
			free(recvTemp);
			recvTemp = NULL;
			// printf("debug: Received from serverC: %s\n",recv_dataC);

			//combine result of ABC
			//if function is search
			if (strcmp(function,"search")==0){
				sendSearchResult(word, recv_dataA, recv_dataB, recv_dataC, new_fd, new_fd_monitor);
			}
			else if (strcmp(function,"suffix")==0 ||strcmp(function,"prefix")==0) {
				sendPrefixOrSuffixResult(word, recv_dataA, recv_dataB, recv_dataC, new_fd, new_fd_monitor);
			}
			
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this

	} // end of while(1)
	return 0;
}