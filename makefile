all: server client
server:
	gcc -g -Wall -o infod infod.c
client:
	gcc -g -Wall -o client client.c
clean:
	rm -rf client
