#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define SOCK_NAME "mysocket.sock"

int sd = -1;

void graceful(int sd)
{
    if (sd != -1)
        close(sd);
    unlink(SOCK_NAME);
}

void catcher(int sig)
{
    graceful(sd);
    printf("\nSignal catcher called for SIGINT: shutting down...\n");
    exit(0);
}

int main()
{
    char buf[100];

    sd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sd == -1)
    {
        perror("socket failed");
        return -1;
    }

    struct sockaddr srvr_name;
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);

    if (bind(sd, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) < 0)
    {
        perror("can't bind()");
        return -1;
    }

    signal(SIGINT, catcher);
    printf("Ready for client to connect().\n");

    while (strcmp(buf, "stop"))
    {
        int bytes = recv(sd, buf, sizeof(buf), 0);
        if (bytes <= 0)
        {
            perror("can't recv()");
            graceful(sd);
            return -1;
        }
        buf[bytes] = 0;
        printf("Server red: %s\n", buf);

        sendto(sd, buf, strlen(buf), 0, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family));
    }

    printf("Server stopped listening\n");
    graceful(sd);
    printf("Socket closed\n");
    return 0;
}