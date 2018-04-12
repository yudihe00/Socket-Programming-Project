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
servera:
	./servera

.PHONY:serverB
servera:
	./serverb

.PHONY:serverC
servera:
	./serverc

.PHONY: monitor
monitor:
	./monitor

