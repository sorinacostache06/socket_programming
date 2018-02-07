#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 40000
#define LOCALHOST "127.0.0.1"
#define PATH_MEMINFO "/home/sacostache/Documents/meminfo"

struct request {
    int reqId;
    int argLen;
    char *argData;
};

struct response {
    int reqId;
    int errCode;
    int respLen;
    char respData[100];
};