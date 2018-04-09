/*
** aws.c
** Yudi He
** ID: 5670857217
UDP port 23217
TCP with client 25217 
TCP with monitor 26217
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

#define BACKLOG 10	 // how many pending connections queue will hold

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
	printf("debug: sockfd is %d\n", sockfd);
	printf("debug: sockfd_monitor is %d\n", sockfd_monitor);
	printf("The AWS is up and running.\n");

	int monitorOn=0;
	while (monitorOn == 0){
		// child socket connect with monitor
		new_fd_monitor = accept(sockfd_monitor, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd_monitor == -1) {
			perror("accept");
			continue;
		}
		printf("debug:monitorOn=%d\n",monitorOn);

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		//printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd_monitor); // child doesn't need the listener
			
			// receive function and word from client
			if ((numbytes = recv(new_fd_monitor, function, sizeof function, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			function[numbytes] = '\0';
			printf("debug: function is %s\n", function);
			//strcpy(function,buf);
			if ((numbytes = recv(new_fd_monitor, word, sizeof word, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			word[numbytes] = '\0';
			printf("debug: word is %s\n", word );
			
			printf("The AWS received input=<%s> and function=<%s> from the client using TCP over port 26217\n",word,function);
			char test[20]="Hello,word!";
			if (send(new_fd_monitor, test, sizeof test, 0) == -1)
				perror("send");
			char test2[20]="Hello,word2!";
			if (send(new_fd_monitor, test2, sizeof test2, 0) == -1)
				perror("send");
			close(new_fd_monitor);
			monitorOn=1;
			printf("debug:monitorOn=%d\n",monitorOn);
			break;
			exit(0);
			
		}
		close(new_fd_monitor);  // parent doesn't need this
	}
	printf("debug:out monitorOn=%d\n",monitorOn);
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
			printf("debug: function is %s\n", function);
			//strcpy(function,buf);
			if ((numbytes = recv(new_fd, word, sizeof word, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			word[numbytes] = '\0';
			printf("debug: word is %s\n", word );
			
			printf("The AWS received input=<%s> and function=<%s> from the client using TCP over port 25217\n",word,function);

			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this

		
		
	} // end of while(1)
	return 0;
}