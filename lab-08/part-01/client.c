#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SOCK_NAME "mysocket.sock"

int main()
{
    int sd;
    struct sockaddr srvr_name;
    char buf[100];

    sd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("can't socket()");
        return -1;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);

    scanf("%99s", buf);
    sendto(sd, buf, strlen(buf), 0, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family));

    printf("Client sent: %s\n", buf);
    return 0;
}