all: 
	gcc -o aws aws.c
	gcc -o client client.c
	# gcc -o listener listener.c
	gcc -o talker talker.c
	gcc -o servera servera.c
	gcc -o monitor monitor.c

.PHONY: aws
aws:
	./aws

.PHONY:serverA
serverA:
	./serverA

clean:
	rm -rf aws
