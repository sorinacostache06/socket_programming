#include "us_xfr.h"
#include <arpa/inet.h>

int main()
{
    int fdc;
    struct sockaddr_in addr;
    struct request req;
    struct response respServer;
    int reqId;
    char cmd[11];

    if ((fdc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("error %d at client socket: %s\n", errno, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, LOCALHOST, &(addr.sin_addr));

    if (connect(fdc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("error %d client connect: %s\n", errno, strerror(errno));
        if (close(fdc) == -1)
            printf("error %d close server file descriptor: %s \n", errno, strerror(errno));
        return -1;
    } else {
        printf("Connected!\n\n");
        printf("Available commands: \n");
        printf("meminfo\n");
        printf("cpuinfo\n");
        printf("disconnect\n\n");
    }

    while(1) {
        memset(&req, 0, sizeof(struct request));
        memset(&respServer, 0, sizeof(struct response));

        printf("Press command: ");
        scanf("%s", cmd);
        printf("\n");

        if (strcmp(cmd, "meminfo") == 0) {
            reqId = 1;
        } else if (strcmp(cmd, "cpuinfo") == 0) {
            reqId = 2;
        } else if (strcmp(cmd, "disconnect") == 0) {
            reqId = 3;
        } else {
            printf("this command does't exit\n");
            reqId = -1;
        }
        
        req.reqId = reqId;
        req.argLen = 0;

        if (write(fdc, &req, sizeof(struct request)) == -1) {
            printf("error no %d when client try to write: %s\n", errno, strerror(errno));
            if (close(fdc) == -1)
                printf("error %d close client file descriptor: %s \n", errno, strerror(errno));
            return -1;
        }

        if (read(fdc, &respServer, sizeof(struct response)) == -1) {
            printf("error no %d when client try to write: %s\n", errno, strerror(errno));
            if (close(fdc) == -1)
                printf("error %d close client file descriptor: %s \n", errno, strerror(errno));
            return -1;
        }

        if (respServer.errCode == SUCCESS) {
            printf("Id request: %d \n",respServer.reqId);
            printf("%s\n",respServer.respData);
        } else if (respServer.errCode == INTERN_ERROR) {
            printf("error processing the request %d\n", respServer.reqId);
        } else if (respServer.errCode == DISCONNECT) {
            printf("Client disconnect!\n");
            return 0;
        } else {
            printf("request %d not implemented\n", respServer.reqId);
        }
    }

    close(fdc);
    return 0;
}
