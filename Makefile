all: 
	gcc -o aws aws.c
	gcc -o client client.c
	# gcc -o listener listener.c
	gcc -o talker talker.c
	gcc -o servera servera.c
	gcc -o monitor monitor.c
	gcc -o searcha searcha.c

.PHONY: aws
aws:
	./aws

.PHONY:serverA
servera:
	./servera

.PHONY: monitor
monitor:
	./monitor

clean:
	rm -rf aws
