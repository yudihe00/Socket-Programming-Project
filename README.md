# ee450-project-18spring     
### What I have done  
	I have completed all parts of requirement including extra points part.     
	1. Phase 1A  
	- All four server programs boot up, listening at specific port, showing boot up message.  
	- Run client program, sending function (search or prefix) and input word to the AWS server over TCP.  
	2. Phase 1B  
	- AWS server send function and word to the three back-servers over UDP.  
	- Each server do their function operation search separately and send results back to AWS server over UDP, displaying their operation states.  
	3. Phase 2  
	 - AWS combines returning result from three back-end servers and send the combined result to monitor and client.  
	4. Phase 3 (**extra points**)  
	- I implement the suffix function which is similar to prefix function but to find the matching words with same prefix.  
	
### Code files  
	***aws.c***  
	Receive function and input from client over TCP, send the function and input to three back-end servers separately over UDP. Combine feedback from three back-end servers and send result to monitor and client over TCP. After booting, the aws server can only be shut down by crtl+c command.  
	***client.c***  
	Send function and input to aws over TCP. Receive feedback from aws and show on screen. The client will terminate itself after receiving feedback from aws.  
	***monitor.c***  
	Booting before running client, show result sent by aws server.  
	***servera.c***  
	Receive function and word from aws over UDP, do the function relative searching in backendA database, send back result to aws.  
	***serverb.c***  
	Receive function and word from aws over UDP, do the function relative searching in backendB database, send back result to aws.  
	***serverc.c***  
	Receive function and word from aws over UDP, do the function relative searching in backendC database, send back result to aws.  

### How to run
	1. Put all files and three database txt files in one directory. Open six terminal in the directory, run each program in the following order on different terminals
	2. run three servers in any order use <make server#>
	3. run aws use <make aws>
	4. run monitor use <make monitor>
	5. Run client use <./client [function (choose one from search, suffix or prefix)] word>
	6. The client.c will terminate by itself, other programs need to use <crtl+c> to terminate.

### Messages exchange format example  
	1. Execute search hack  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<hack> and function=<search> from the client using TCP over port 25217  
	Sent <search> and <hack> to Backend-Server A  
	Sent <search> and <hack> to Backend-Server B  
	Sent <search> and <hack> to Backend-Server C  
	The AWS received <1> similar words from Backend-Server <A> using UDP over port <21217>  
	The AWS received <1> similar words from Backend-Server <B> using UDP over port <22217>  
	The AWS received <1> similar words from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <1> matches to client  
	The AWS sent <hack> and <Jack> to client to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <hack> and <search> to AWS.  
	Found a match for < hack >:  
	< Hackneyed; hired; mercenary. >  
	- monitor console  
	The monitor is up and running.  
	Found a match for < hack >:  
	< Hackneyed; hired; mercenary. >  
	One edit distance match is <Jack>:  
	<"To wash on a rack  as metals or ore.">  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <search> and operation <hack>  
	The Server A has found < 0 > matches and < 1 > similar words  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <search> and operation <hack>  
	The Server B has found < 1 > matches and < 1 > similar words  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <search> and operation <hack>  
	The Server C has found < 1 > matches and < 1 > similar words  
	The Server C finished sending the output to AWS  
	
	2. Execute <prefix> <accuse>  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<accus> and function=<prefix> from the client using TCP over port 25217  
	Sent <prefix> and <accus> to Backend-Server A  
	Sent <prefix> and <accus> to Backend-Server B  
	Sent <prefix> and <accus> to Backend-Server C  
	The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  
	The AWS received <3> matches from Backend-Server <B> using UDP over port <22217>  
	The AWS received <3> matches from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <7> matches to client  
	The AWS sent <7> matches to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <accus> and <prefix> to AWS.  
	Found <7> matches for <accus>:  
	<Accustomed>  
	<Accuser>  
	<Accuse>  
	<Accusatorially>  
	<Accustom>  
	<Accuse>  
	<Accusement>  
	- monitor console  
	The monitor is up and running.  
	Found <7> matches for <accus>:  
	<Accustomed>  
	<Accuser>  
	<Accuse>  
	<Accusatorially>  
	<Accustom>  
	<Accuse>  
	<Accusement>  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <prefix> and operation <accus>  
	The Server A has found < 1 > matches  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <prefix> and operation <accus>  
	The Server B has found < 3 > matches  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <prefix> and operation <accus>  
	The Server C has found < 3 > matches  
	The Server C finished sending the output to AWS  
	
	3. Execute <suffix> <ntable>  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<ntable> and function=<suffix> from the client using TCP over port 25217  
	Sent <suffix> and <ntable> to Backend-Server A  
	Sent <suffix> and <ntable> to Backend-Server B  
	Sent <suffix> and <ntable> to Backend-Server C  
	The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  
	The AWS received <1> matches from Backend-Server <B> using UDP over port <22217>  
	The AWS received <2> matches from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <4> matches to client  
	The AWS sent <4> matches to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <ntable> and <suffix> to AWS.  
	Found <4> matches for <ntable>:  
	<Replantable>  
	<Acquaintable>  
	<Accountable>  
	<Fermentable>  
	- monitor console  
	The monitor is up and running.  
	Found <4> matches for <ntable>:  
	<Replantable>  
	<Acquaintable>  
	<Accountable>  
	<Fermentable>  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <suffix> and operation <ntable>  
	The Server A has found < 1 > matches  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <suffix> and operation <ntable>  
	The Server B has found < 1 > matches  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <suffix> and operation <ntable>  
	The Server C has found < 2 > matches  
	The Server C finished sending the output to AWS  

### Idiosyncrasy of my project.  
	I set maximum size of the TCP and UTP buffer as 4000, if the searching result of prefix and suffix is very long, such as the input word only has one character the memory will crash.  

### Reused Code  
	My TCP and UDP setting up code is based on examples in Beej's book.  
