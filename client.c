#include "us_xfr.h"
#include <arpa/inet.h>
#include <sys/sysinfo.h>

int main()
{
    int fdc;
    struct sockaddr_in addr;
    struct request req;
    struct response respServer;
    struct sysinfo info;
    int reqId, argLen;
    char *argData;

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
        return -1;
    } else {
        printf("Connected!\n");
    }

    while(1) {
        memset(&req, 0, sizeof(struct request));
        memset(&respServer, 0, sizeof(struct response));
        
        printf("Press request id: ");
        scanf("%d", &reqId);
        // printf("\nPress argument: ");
        // scanf("%s", &argData);
        printf("\n");
        
        // if (strcmp(argData,"") == 0) 
        //     argLen = 0;

        req.reqId = reqId;
        req.argLen = 0;

        if (write(fdc, &req, sizeof(struct request)) == -1) {
            printf("write error in client\n");
        }

        if (req.reqId == 1) {
            sysinfo(&info);
            printf("%ld\n", info.totalram);
            printf("%ld\n", info.freeram);
        }
        // read(fdc,recvline,100);
        // printf("%s",recvline);
    }

    close(fdc);
}