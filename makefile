all: infod client
infod:
	gcc -g -Wall -o infod infod.c
client:
	gcc -g -Wall -o client client.c
clean:
	rm -rf client
	rm -rf infod