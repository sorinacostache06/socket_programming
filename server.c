#include "us_xfr.h"
#include <sys/sysinfo.h>

struct sysinfo info;

struct response meminfo(int id)
{
    struct response aux;
    unsigned long totalRam; 
    unsigned long freeRam;
    char *data;
    sysinfo(&info);
    strcat(data, "total memory: ");
    strcat(data, totalRam);
    strcat(data, " free memory: ");
    strcat(data, freeRam);

    aux.reqId = id;
    aux.errCode = 0;
    aux.respLen = strlen(data);
    strcpy(aux.respData, data);
    
    return aux;
}

int main()
{
    int fds, new_fd;
    struct sockaddr_in addr;
    struct request reqClient;
    struct response resp;

    if ((fds = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Server socket number error: %d %s \n", errno, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(fds, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
        printf("Server socket bind number error: %d %s \n", errno, strerror(errno));
        return -1;
    }

    if (listen(fds, 10) == -1) {
        printf("error no %d at server listen: %s\n", errno, strerror(errno));
        return -1;
    }

    new_fd = accept(fds, (struct sockaddr*) NULL, NULL);

    while(1) {
        memset(&reqClient, 0, sizeof(struct request));
        memset(&resp, 0, sizeof(struct response));
        printf("The server process your request!\n");
        if (read(new_fd, &reqClient, sizeof(struct request)) == -1) {
            printf("read error in server");
        }

        if (reqClient.reqId == 1) {
            resp = meminfo(reqClient.reqId);
        }
        // printf("Echoing back - %d",reqClient.reqId);
        // write(new_fd, , strlen(str)+1);
    }

    close(fds);
}