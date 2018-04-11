/*
** talker.c -- a datagram "client" demo
change talker.c to input function(search or prefix) word
get feedback from listener
20180405

prototype of aws UDP client part 
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

#define SERVERPORTA "21217"	// the port users will be connecting to 
							// Backend-Server (A)
#define SERVERPORTB "22217"	// the port users will be connecting to 
							// Backend-Server (B)
#define SERVERPORTC "23217"	// the port users will be connecting to 
							// Backend-Server (C)
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
   	//printf("debug in udpQuery: Received :%s, bytes_recv is %d\n",recv_data,bytes_recv);
	close(sockfd);
	strcpy(return_recv_data,recv_data);
	return return_recv_data;
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

// // Convert string to int
// int stringToInt(const char* snum){
// 	int dec;
// 	int len = strlen(snum);
// 	for(int i=0; i<len; i++){
// 		dec = dec * 10 + ( snum[i] - '0' );
// 	}
// 	return dec;
// }

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
	printf("debug: number of similar word, tempString is %s, number is %d\n",tempString,returnResult.similarNumber);
	
	if(returnResult.similarNumber==0) return returnResult; // No similar word
	else { // have similar words
		// // put all similar words in one string
		// currentIndex=lastIndexOfSubstring+4; // First index of the first similar word
		// lastIndexOfSubstring=lastIndexOfStringSeg(recv_data,currentIndex);
		// int wordLength = lastIndexOfSubstring - currentIndex + 1;
		// printf("debug: wordLength is %d\n",wordLength);
		// lastIndexOfSubstring = currentIndex-1 + (wordLength+3) * returnResult.similarNumber;
		// strncpy(returnResult.similarWords,recv_data+currentIndex,lastIndexOfSubstring - currentIndex+1);
		// printf("debug: similarWords string is %s\n", returnResult.similarWords);
		
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
		printf("debug: definition is <%s>\n", returnResult.definition);

		// get similar word number 
		currentIndex = lastIndexOfSubstring+4;
		returnResult = similarResultInOneSearch(currentIndex, returnResult,recv_data);

	}
	return returnResult;
}

// send search result to client and monitor
void sendSearchResult(char* word, char* recv_dataA, char* recv_dataB, char* recv_dataC)
{
	char matchNumberString[10]="0"; //number of same words
	char similarNumberString[10]="0";
	char definition[101]=""; //definition of match word
	char oneSimilarWord[100]="0";
	char oneSimilarWordDifinition[101]=""; //difinition of one of the similar word
	// char similarWordString[4000];
	char returnString[300]=""; // the result sendback to client and monitor
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
	printf("debug: the return string is %s\n", returnString);
	//return returnString;

	// printf("debug: test for B, matchNumber for non exist word is %d\n",resultB.matchNumber);
	// printf("debug: test for B, one similar word is <%s>, definition is <%s>\n",resultB.oneSimilarWord, resultB.oneSimilarWordDifinition);

}

int main(int argc, char *argv[])
{
	int sockfd;	
	int numbytes;
	char function[7]; //function sent by client
	char word[100]; //word sent by client
	char returnString[300]=""; // the result sendback to client and monitor

	if (argc != 3) {
		fprintf(stderr,"usage: talker function word\n");
		exit(1);
	}
	strcpy(function,argv[1]);
	strcpy(word,argv[2]);
	char *recv_dataA=udpQuery(function,word,SERVERPORTA);
	char *recv_dataB=udpQuery(function,word,SERVERPORTB);
	char *recv_dataC=udpQuery(function,word,SERVERPORTC);
	printf("ReceivedA :%s\n",recv_dataA);
	printf("ReceivedB :%s\n",recv_dataB);
	printf("ReceivedC :%s\n",recv_dataB);

	//combine result of ABC
	//if function is search
	if (strcmp(function,"search")==0){
		//strcpy(returnString, getSearchResultString(word, recv_dataA, recv_dataB, recv_dataC));
		//printf("debug: the final return string is %s\n",returnString);
		sendSearchResult(word, recv_dataA, recv_dataB, recv_dataC);
	}
	return 0;
}
