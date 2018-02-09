#include "us_xfr.h"
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <fcntl.h>

FILE *fdlog;

int meminfo(char **memory)
{
    char buff[100];

    FILE* file;
    if ((file = fopen("/proc/meminfo", "r")) == NULL) {
        fprintf(fdlog, "error %d when open meminfo file: %s \n", errno, strerror(errno));
        fclose(file);
        return -1;
    }

    *memory = malloc(100 * sizeof(char));
    strcpy(*memory,"");
    for (int i = 0; i < 2; i++) {
        if (fgets(buff, sizeof(buff), file) != NULL) {
             buff[strlen(buff) - 1] = '\0'; 
        } else {
            fprintf(fdlog, "error %d when open stat file: %s \n", errno, strerror(errno));
            fclose(file);
            return -1;
        }     
        strcat(*memory, buff);
        if (i == 0)
            strcat(*memory, "\n");
    } 
    fclose(file);
    return 0;
}

int cpuvalues(int cpu0[10])
{
    char buff[100];
    FILE* file; 

    if ((file = fopen("/proc/stat", "r")) == NULL) {
        fprintf(fdlog, "error %d when open stat file: %s \n", errno, strerror(errno));
        return -1;
    }

    if (fgets(buff, sizeof(buff), file) != NULL) {
        buff[strlen(buff) - 1] = '\0'; 
    } else {
        fclose(file);
        return -1;
    }

    char *split;
    int i = 0;
    split = strtok(buff," ");
    while (split != NULL) {
        cpu0[i++] = atoi(split);
        split = strtok (NULL, " ,.-");
    }
    fclose(file);
    return 0;
}

float cpuinfo()
{
    int cpu0[12], cpu1[12];
    float total0 = 0, total1 = 0, work0 = 0, work1 = 0, cpu;
    
    if (cpuvalues(cpu0) == -1) {
        fprintf(fdlog, "error: can't get cpu values\n");
        return -1;
    }

    sleep(1);

    if (cpuvalues(cpu1) == -1) {
        fprintf(fdlog, "error: can't get cpu values\n");
        return -1;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 1 || i == 2 || i == 3) {
            work0 += cpu0[i];
            work1 += cpu1[i];
        }
        total0 += cpu0[i];
        total1 += cpu1[i];
    }

    cpu = ((work1 - work0)/(total1 - total0))*100;
    return cpu;
}

void setResponse(struct response *resp, int id, int error, char *data)
{
    resp->reqId = id;
    resp->errCode = error;
    resp->respLen = strlen(data);
    strcpy(resp->respData, data);
}

int becomeDaemon()
{
    int fd;

    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) {
        printf("error %d setsid: %s\n", errno, strerror(errno));
        return -1;
    }

    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    umask(0);

    if (chdir("/") == -1) {
        printf("error %d when try tochange directory: %s\n", errno, strerror(errno));
        return -1;
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if ((fd = open("/dev/null", O_RDWR)) == -1) {
        printf("error %d when try open /dev/null: %s\n", errno, strerror(errno));
        return -1;
    }

    if (fd != STDIN_FILENO)        
        return -1;
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        return -1;
    
    if ((fdlog = fopen(LOGFILE, "w+")) == NULL) {
        printf("error %d open logging file: %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}

int main()
{
    int fds, new_fd, flag;
    float cpu;
    char *data;
    struct sockaddr_in addr;
    struct request reqClient;
    struct response resp;
    char buff[5];

    if (becomeDaemon() == -1) {
        printf("Can't create daemon!\n");
        return -1;
    }

    if ((fds = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(fdlog, "Server socket number error: %d %s \n", errno, strerror(errno));
        close(fds);
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(fds, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(fdlog, "Server socket bind number error: %d %s \n", errno, strerror(errno));
        close(fds);
        return -1;
    }

    if (listen(fds, 10) == -1) {
        fprintf(fdlog, "error no %d at server listen: %s\n", errno, strerror(errno));
        close(fds);
        return -1;
    }

    while(1) {
        flag = 0;
        
        if ((new_fd = accept(fds, (struct sockaddr*) NULL, NULL)) == -1) {
            fprintf(fdlog, "error no %d at server accept: %s\n", errno, strerror(errno));
            close(fds);
            return -1;
        }

        while(flag == 0) {
            memset(&reqClient, 0, sizeof(struct request));
            memset(&resp, 0, sizeof(struct response));

            if (read(new_fd, &reqClient, sizeof(struct request)) == -1) {
                fprintf(fdlog, "error no %d when server try to read: %s\n", errno, strerror(errno));
                close(new_fd);
                return -1;
            }

            data = malloc(100 * sizeof(char));

            switch(reqClient.reqId) {
                case 1: 
                    strcpy(data, "");
                    if (meminfo(&data) == -1) {
                        fprintf(fdlog, "error: can't process meminfo");
                        setResponse(&resp, reqClient.reqId, INTERN_ERROR, "");
                    } else {
                        setResponse(&resp, reqClient.reqId, SUCCESS, data);
                    }
                    break;
                case 2:
                    if ((cpu = cpuinfo()) == -1) {
                        fprintf(fdlog, "error: can't process cpuinfo");
                        setResponse(&resp, reqClient.reqId, INTERN_ERROR, "");
                    } else {
                        sprintf(buff, "%.2f", cpu);
                        setResponse(&resp, reqClient.reqId, SUCCESS, buff);
                    }
                    break;
                case 3:
                    setResponse(&resp, reqClient.reqId, DISCONNECT, "");
                    break;
                default: 
                    setResponse(&resp, reqClient.reqId, EXTERN_ERROR, "");
                    break;
            }

            if (write(new_fd, &resp, sizeof(struct response)) == -1) {
                fprintf(fdlog, "error no %d when server try to write: %s\n", errno, strerror(errno));
                close(new_fd);
                return -1;
            }

            if (resp.errCode == DISCONNECT) {
                close(new_fd);
                flag = 1;
            }
        }       
    }

    close(fds);
    return 0;
}
