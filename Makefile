####### Make file
# Yudi He
# ID: 5670857217
# Use <make> to compile
####### How to run:
# Open six terminal, run each program in the following order on different terminal
# 1. run aws use <make aws>
# 2. run monitor, three servers in any order use <make monitor>,<make server#>
# 3. run client use <./client [function (choose one from search, suffix or prefix)] word>
# 4. client.c will terminate by itself, other programs need to use <crtl+c> to terminate.
#######

all: 
	gcc -o aws aws.c
	gcc -o client client.c
	# gcc -o listener listener.c
	# gcc -o talker talker.c
	gcc -o servera servera.c
	gcc -o serverb serverb.c
	gcc -o serverc serverc.c
	gcc -o monitor monitor.c

.PHONY: aws
aws:
	./aws

.PHONY:serverA
serverA:
	./servera

.PHONY:serverB
serverB:
	./serverb

.PHONY:serverC
serverC:
	./serverc

.PHONY: monitor
monitor:
	./monitor

