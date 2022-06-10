#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <netdb.h>

#define MSG_LEN 256
#define SOCK_ADDR "localhost"
#define SOCK_PORT 8080

int main(void)
{
    int sd, pid;
    struct hostent *host;
    struct sockaddr_in addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("can't socket(): ");
        return sd;
    }

    host = gethostbyname(SOCK_ADDR);
    if (!host)
    {
        perror("can't gethostbyname(): ");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("can't connect() ");
        return -1;
    }

    pid = getpid();

    char msg[MSG_LEN];
    for (int i = 0; i < 4; i++)
    {
        memset(msg, 0, MSG_LEN);
        sprintf(msg, "Я тебя ненавижу from %d", pid);

        if (send(sd, msg, strlen(msg), 0) < 0)
        {
            perror("can't send()");
            return -1;
        }

        printf("[%d] sent msg #%d: %s\n", pid, i, msg);
        sleep(5);
    }

    printf("Client [%d] terminated.\n", pid);
    return 0;
}