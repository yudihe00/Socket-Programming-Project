# ee450-project-18spring  \<br>
Name: Yudi He    \<br>
ID: 5670857217    \<br>
### What I have done  \<br>
I have completed all parts of requirement including extra points part.     \<br>
1.  Phase 1A  \<br>
- All four server programs boot up, listening at specific port, showing boot up message.  \<br>
 - Run client program, sending function (search or prefix) and input word to the AWS server over TCP.  \<br>
2. Phase 1B  \<br>
- AWS server send function and word to the three back-servers over UDP.  \<br>
- Each server do their function operation search separately and send results back to AWS server over UDP, displaying their operation states.  \<br>
3. Phase 2  \<br>
 - AWS combines returning result from three back-end servers and send the combined result to monitor and client.  \<br>
4. Phase 3 (**extra points**)  \<br>
- I implement the suffix function which is similar to prefix function but to find the matching words with same prefix.  \<br>
### Code files  \<br>
***aws.c***  \<br>
Receive function and input from client over TCP, send the function and input to three back-end servers separately over UDP. Combine feedback from three back-end servers and send result to monitor and client over TCP. After booting, the aws server can only be shut down by crtl+c command.  \<br>
***client.c***  \<br>
Send function and input to aws over TCP. Receive feedback from aws and show on screen. The client will terminate itself after receiving feedback from aws.  \<br>
***monitor.c***  \<br>
Booting before running client, show result sent by aws server.  \<br>
***servera.c***  \<br>
Receive function and word from aws over UDP, do the function relative searching in backendA database, send back result to aws.  \<br>
***serverb.c***  \<br>
Receive function and word from aws over UDP, do the function relative searching in backendB database, send back result to aws.  \<br>
***serverc.c***  \<br>
Receive function and word from aws over UDP, do the function relative searching in backendC database, send back result to aws.  \<br>
### Messages exchange format example  \<br>
1. Execute search hack  \<br>
- aws console  \<br>
The AWS is up and running.  \<br>
The AWS received input=<hack> and function=<search> from the client using TCP over port 25217  \<br>
Sent <search> and <hack> to Backend-Server A  \<br>
Sent <search> and <hack> to Backend-Server B  \<br>
Sent <search> and <hack> to Backend-Server C  \<br>
The AWS received <1> similar words from Backend-Server <A> using UDP over port <21217>  \<br>
The AWS received <1> similar words from Backend-Server <B> using UDP over port <22217>  \<br>
The AWS received <1> similar words from Backend-Server <C> using UDP over port <23217>  \<br>
The AWS sent <1> matches to client  \<br>
The AWS sent <hack> and <Jack> to client to the monitor via TCP port <26217>  \<br>
- client console  \<br>
The client is up and running.”  \<br>
The client sent <hack> and <search> to AWS.  \<br>
Found a match for < hack >:  \<br>
< Hackneyed; hired; mercenary. >  \<br>
- monitor console  \<br>
The monitor is up and running.  \<br>
Found a match for < hack >:  \<br>
< Hackneyed; hired; mercenary. >  \<br>
One edit distance match is <Jack>:  \<br>
<"To wash on a rack  as metals or ore.">  \<br>
- servera console  \<br>
The Server A is up and running using UDP on port <21217>.  \<br>
The Server A received input <search> and operation <hack>  \<br>
The Server A has found < 0 > matches and < 1 > similar words  \<br>
The Server A finished sending the output to AWS  \<br>
- serverb console  \<br>
The Server B is up and running using UDP on port <22217>.  \<br>
The Server B received input <search> and operation <hack>  \<br>
The Server B has found < 1 > matches and < 1 > similar words  \<br>
The Server B finished sending the output to AWS  \<br>
- serverc console  \<br>
The Server C is up and running using UDP on port <23217>.  \<br>
The Server C received input <search> and operation <hack>  \<br>
The Server C has found < 1 > matches and < 1 > similar words  \<br>
The Server C finished sending the output to AWS  \<br>
2. Execute <prefix> <accuse>  \<br>
- aws console  \<br>
The AWS is up and running.  \<br>
The AWS received input=<accus> and function=<prefix> from the client using TCP over port 25217  \<br>
Sent <prefix> and <accus> to Backend-Server A  \<br>
Sent <prefix> and <accus> to Backend-Server B  \<br>
Sent <prefix> and <accus> to Backend-Server C  \<br>
The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  \<br>
The AWS received <3> matches from Backend-Server <B> using UDP over port <22217>  \<br>
The AWS received <3> matches from Backend-Server <C> using UDP over port <23217>  \<br>
The AWS sent <7> matches to client  \<br>
The AWS sent <7> matches to the monitor via TCP port <26217>  \<br>
- client console  \<br>
The client is up and running.”  \<br>
The client sent <accus> and <prefix> to AWS.  \<br>
Found <7> matches for <accus>:  \<br>
<Accustomed>  \<br>
<Accuser>  \<br>
<Accuse>  \<br>
<Accusatorially>  \<br>
<Accustom>  \<br>
<Accuse>  \<br>
<Accusement>  \<br>
- monitor console  \<br>
The monitor is up and running.  \<br>
Found <7> matches for <accus>:  \<br>
<Accustomed>  \<br>
<Accuser>  \<br>
<Accuse>  \<br>
<Accusatorially>  \<br>
<Accustom>  \<br>
<Accuse>  \<br>
<Accusement>  \<br>
- servera console  \<br>
The Server A is up and running using UDP on port <21217>.  \<br>
The Server A received input <prefix> and operation <accus>  \<br>
The Server A has found < 1 > matches  \<br>
The Server A finished sending the output to AWS  \<br>
- serverb console  \<br>
The Server B is up and running using UDP on port <22217>.  \<br>
The Server B received input <prefix> and operation <accus>  \<br>
The Server B has found < 3 > matches  \<br>
The Server B finished sending the output to AWS  \<br>
- serverc console  \<br>
The Server C is up and running using UDP on port <23217>.  \<br>
The Server C received input <prefix> and operation <accus>  \<br>
The Server C has found < 3 > matches  \<br>
The Server C finished sending the output to AWS  \<br>
3. Execute <suffix> <ntable>  \<br>
- aws console  \<br>
The AWS is up and running.  \<br>
The AWS received input=<ntable> and function=<suffix> from the client using TCP over port 25217  \<br>
Sent <suffix> and <ntable> to Backend-Server A  \<br>
Sent <suffix> and <ntable> to Backend-Server B  \<br>
Sent <suffix> and <ntable> to Backend-Server C  \<br>
The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  \<br>
The AWS received <1> matches from Backend-Server <B> using UDP over port <22217>  \<br>
The AWS received <2> matches from Backend-Server <C> using UDP over port <23217>  \<br>
The AWS sent <4> matches to client  \<br>
The AWS sent <4> matches to the monitor via TCP port <26217>  \<br>
- client console  \<br>
The client is up and running.”  \<br>
The client sent <ntable> and <suffix> to AWS.  \<br>
Found <4> matches for <ntable>:  \<br>
<Replantable>  \<br>
<Acquaintable>  \<br>
<Accountable>  \<br>
<Fermentable>  \<br>
- monitor console  \<br>
The monitor is up and running.  \<br>
Found <4> matches for <ntable>:  \<br>
<Replantable>  \<br>
<Acquaintable>  \<br>
<Accountable>  \<br>
<Fermentable>  \<br>
- servera console  \<br>
The Server A is up and running using UDP on port <21217>.  \<br>
The Server A received input <suffix> and operation <ntable>  \<br>
The Server A has found < 1 > matches  \<br>
The Server A finished sending the output to AWS  \<br>
- serverb console  \<br>
The Server B is up and running using UDP on port <22217>.  \<br>
The Server B received input <suffix> and operation <ntable>  \<br>
The Server B has found < 1 > matches  \<br>
The Server B finished sending the output to AWS  \<br>
- serverc console  \<br>
The Server C is up and running using UDP on port <23217>.  \<br>
The Server C received input <suffix> and operation <ntable>  \<br>
The Server C has found < 2 > matches  \<br>
The Server C finished sending the output to AWS  \<br>
### Idiosyncrasy of my project.  \<br>
I set maximum size of the TCP and UTP buffer as 4000, if the searching result of prefix and suffix is very long, such as the input word only has one character the memory will crash.  \<br>
### Reused Code  \<br>
My TCP and UDP setting up code is based on examples in Beej's book.  \<br>