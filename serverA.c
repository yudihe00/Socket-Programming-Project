/*
** server.c 
based on datagram sockets "server" demo, listener.c in beej
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

// parse line
// return word's last character index
int parseLine(char line[101]){
    int i=0;
    while(line[i]!=' '){
        i++;
    }
    return i-1;
}

// search function
// in function return to aws that definition, similar words number and similar words
void search(char word[],FILE* file)
{
    char line[101];
    int wordLength = strlen(word);
    //printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char definition[100];
    char definition2[100];
    char similarWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int similarWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        //printf("debug:last index is %d\n",lastIndex);

        if (lastIndex+1==wordLength) //have same length
        {
            int same=1;
            int diffNumber=0;
            char dicWord[20];
            for(int i=0; i<=lastIndex; i++) //judge is every characters are same
            {   
                dicWord[i]=line[i];
                if(line[i]!=word[i]) {
                    same=0;
                    diffNumber++;
                }
            }
            dicWord[lastIndex+1]='\0';
            //printf("debug:same is %d\n",same);
            if (same==1)  
            {
                if (findSame!=1) // if find same word, only return the first definition
                {
                    findSame=1;
                    strncpy(definition,line+lastIndex+5,101-lastIndex);
                    //printf("definition is %s\n",definition);
                }
                
            } else if (diffNumber==1){
                strcpy(similarWord[similarWordNumber],dicWord);
                strncpy(definition2,line+lastIndex+5,101-lastIndex); //return the last definition for long slimilar words
                //printf("debug: similarWord[%d] the similar word is %s\n",similarWordNumber,similarWord[similarWordNumber] );
                //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
                similarWordNumber++;
            }
        }
    }
    printf("debug: similar word number is %d\n", similarWordNumber);
    if (findSame ==1){
        printf("definition is <%s",definition);
    } else {
        printf("No same words in this server\n");
    }
    int i;
    for (i=0; i<similarWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, similarWord[i]);
    }
    if (similarWordNumber!=0){
        printf("One edit distance match is <%s>, definition is <%s",similarWord[i-1], definition2);
    }
    //printf("debug:word is %s\n",word);
}

// prefix function
// in function return to aws that words number and words
void prefix(char word[],FILE* file)
{
    char line[101];
    int wordLength = strlen(word);
    // printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char prefixWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int prefixWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        // printf("debug:last index is %d\n",lastIndex);

        int same=1;
        
        char dicWord[20];
        for(int i=0; i<wordLength; i++) //judge every characters are same for word given
        {   
            dicWord[i]=line[i];
            if(line[i]!=word[i]) {
                same=0;
            }
        }
        for (int i=wordLength; i<=lastIndex; i++)
        {
            dicWord[i]=line[i];
        }
        dicWord[lastIndex+1]='\0';
        // printf("debug:dicWord is %s\n",dicWord);
        // printf("debug:same is %d\n",same);
        if (same==1)  
        {
            strcpy(prefixWord[prefixWordNumber],dicWord);
            //printf("debug: similarWord[%d] the similar word is %s\n",similarWordNumber,similarWord[similarWordNumber] );
            //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
            prefixWordNumber++;
        }
        
    }
    printf("debug: prefix word number is %d\n", prefixWordNumber);
    
    int i;
    for (i=0; i<prefixWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, prefixWord[i]);
    }

    if (prefixWordNumber==0) {
        printf("debug: no prefix word find in serverA\n");
    }
    // if (similarWordNumber!=0){
    //     printf("One edit distance match is %s, definition is %s",similarWord[i-1], definition2);
    // }
    //printf("debug:word is %s\n",word);
}

// suffix function
// in function return to aws that words number and words
void suffix(char word[],FILE* file)
{
    char line[101];
    if(word[0]>='A' && word[0]<='Z') word[0]=word[0]-('A'-'a');
    if(line[0]>='a' && line[0]<='z') line[0]=line[0]+('A'-'a');

    int wordLength = strlen(word);
    //printf("debug:word length is %d\n",wordLength);
    int findSame=0;
    char suffixWord[100][20];
    //char* similarWord= (char *) malloc(100);
    //char* s = (char *) malloc(15); 
    int suffixWordNumber=0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        //printf("%s", line); 
        int lastIndex=parseLine(line);
        // printf("debug:last index is %d\n",lastIndex);

        int same=1;
        
        char dicWord[20];
        int sub = lastIndex - wordLength + 1;
        int i;
        for(i=wordLength-1; i>=0; i--) //judge every characters are same for word given
        {   
            dicWord[i+sub]=line[i+sub];
            if(line[i+sub]!=word[i]) {
                same=0;
            }
        }
        for (i=i+sub; i>=0; i--)
        {
            dicWord[i]=line[i];
        }
        dicWord[lastIndex+1]='\0';
        //printf("debug:dicWord is %s\n",dicWord);
        // printf("debug:same is %d\n",same);
        if (same==1 && (dicWord[0]>='A' && dicWord[0]<='Z'))  
        {
            strcpy(suffixWord[suffixWordNumber],dicWord);
            //printf("debug: suffixWord[%d] the suffix word is %s dicWord is %s\n",suffixWordNumber,suffixWord[suffixWordNumber],dicWord );
            //printf("debug: similarWord[0] the similar word is %s\n",similarWord[0] );
            suffixWordNumber++;
        }
        
    }
    printf("debug: suffix word number is %d\n", suffixWordNumber);
    
    int i;
    for (i=0; i<suffixWordNumber; i++) {
        printf("debug: i=%d, the final similar word is %s\n", i, suffixWord[i]);
    }

    if (suffixWordNumber==0) {
        printf("debug: no suffix word find in serverA\n");
    }
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

	char *fileName="backendA.txt";
     

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

	printf("The ServerA is up and running using UDP on port <21217>.\n");

	while(1) {
		
		numbytes = recvfrom(sockfd, function, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len);

		function[numbytes] = '\0';

		numbytes = recvfrom(sockfd, word, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len);

		word[numbytes] = '\0';

		if (numbytes==-1) {
    		perror("recv");
	    	exit(1);
    	}

		//printf("listener: packet is %d bytes long\n", numbytes);
		printf("The ServerA received input <%s> and operation <%s>\n", function,word);
		
		FILE* file = fopen(fileName, "r");
		// change first letter of word to big case
	    if(word[0]>='a' && word[0]<='z') word[0]=word[0]+('A'-'a');
	   
	    if (strcmp(function,"search")==0) search(word, file);
	    else if (strcmp(function,"prefix")==0) prefix(word, file);
	    else if (strcmp(function,"suffix")==0) suffix(word,file);

		fclose(file);

		// send back to aws
		strcpy(send_data,"test: search results from serverA");
    	printf(" ServerA SEND : %s\n",send_data);
    	
    	numbytes = sendto(sockfd,send_data,strlen(send_data),0,
    		(struct sockaddr *)&their_addr, addr_len);
    	
    	//printf("debug: numbytes is %d\n", numbytes);
    	
    	fflush(stdout); //wait for next connect

		//close(sockfd);
	}

	return 0;
}
