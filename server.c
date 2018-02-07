#include "us_xfr.h"
#include <sys/sysinfo.h>

char memory[100];

int meminfo()
{
    char *cmd;
    char buff[100];

    FILE* file;
    if ((file = fopen(PATH_MEMINFO, "a+")) == NULL) {
        printf("error %d in fopen: %s \n", errno, strerror(errno));
        return -1;
    }

    cmd = malloc(strlen("cat /proc/meminfo") + 1);
    strcpy(cmd, "cat /proc/meminfo > ");
    strcat(cmd, PATH_MEMINFO);

    if (system(cmd) != 0) {
        printf("error execute system() in server");
        return -1;
    }

    strcpy(memory,"");
    for (int i = 0; i < 2; i++) {
        if (fgets(buff, sizeof(buff), file) != NULL)
            buff[strlen(buff) - 1] = '\0'; 
        strcat(memory, buff);
        printf("result: %s\n", memory);
        if (i == 0)
            strcat(memory, "\n");
        printf("result: %s\n", memory);
    } 

    printf("result: %s\n", memory);
    return 0;
}

void cpuvalues(int cpu0[10])
{
    char buff[100];
    FILE* file; 

    if ((file = fopen("/proc/stat", "r")) == NULL) {
        printf("error %d in fopen: %s \n", errno, strerror(errno));
    }

    if (fgets(buff, sizeof(buff), file) != NULL)
            buff[strlen(buff) - 1] = '\0'; 

    printf("CPU %s\n", buff);
    char *split;
    int i = 0;
    split = strtok(buff," ");
    while (split != NULL) {
        cpu0[i++] = atoi(split);
        split = strtok (NULL, " ,.-");
    }
}

float cpuinfo()
{
    int cpu0[12], cpu1[12];
    float total0 = 0, total1 = 0, work0 = 0, work1 = 0, cpu;
    
    cpuvalues(cpu0);
    sleep(1);
    cpuvalues(cpu1);

    for (int i = 0; i < 10; i++) {
        if (i == 1 || i == 2 || i == 3) {
            work0 += cpu0[i];
            work1 += cpu1[i];
        }
        total0 += cpu0[i];
        total1 += cpu1[i];
    }

    cpu = ((work1 - work0)/(total1 - total0))*100;
    // printf("cpu %f\n", cpu);
    return cpu;
}

void setResponse(struct response *resp, int id, int error, float cpu)
{
    resp->reqId = id;
    resp->errCode = 0;
    resp->respLen = strlen(memory);
    // resp->respData = malloc(strlen(memory) + 1);
    strcpy(resp->respData, memory);
}

int main()
{
    int fds, new_fd;
    float cpu;
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

        switch(reqClient.reqId) {
            case 1: 
                if (meminfo() == -1) 
                    printf("error: can't process meminfo");
                setResponse(&resp, reqClient.reqId, 0, 0.0);
                break;
            case 2:
                cpu = cpuinfo();
                setResponse(&resp, reqClient.reqId, 0, cpu);
                break;
            default: 
                setResponse(&resp, reqClient.reqId, 100, 0.0);
                break;
        }

        // printf("Server response id: %d\n",resp.reqId);
        // printf("Server response error: %d\n",resp.errCode);
        // printf("Server response length: %d\n",resp.respLen);
        // printf("Server response data: %s\n",resp.respData);
        write(new_fd, &resp, sizeof(struct response));
    }

    close(fds);
}