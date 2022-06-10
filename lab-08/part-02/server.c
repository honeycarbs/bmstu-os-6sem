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

#define MAX_CLIENTS 10

int clients[MAX_CLIENTS] = {0};

void connection_handler(int sd)
{
    struct sockaddr_in addr;
    int addr_size, incom, i;

    addr_size = sizeof(addr);

    incom = accept(sd, (struct sockaddr *)&addr, (socklen_t *)&addr_size);
    if (incom < 0)
    {
        perror("can't accept()");
        exit(-1);
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == 0)
        {
            clients[i] = incom;
            break;
        }
    }

    printf("\nNew connection\nClient #%d: %s:%d\n\n",
           i, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}

void client_handler(int sd, int client_id)
{
    char msg[MSG_LEN];
    struct sockaddr_in addr;
    int rc, addr_size;

    memset(msg, 0, MSG_LEN);

    addr_size = sizeof(addr);

    rc = recv(sd, msg, MSG_LEN, 0);
    if (rc == 0)
    {
        getpeername(sd, (struct sockaddr *)&addr, (socklen_t *)&addr_size);
        printf("\nClient #%d disconnected\n\n", client_id);
        close(sd);
        clients[client_id] = 0;
    }
    else
    {
        msg[rc] = '\0';
        printf("Message from client #%d: %s\n", client_id, msg);
    }
}

int main(void)
{
    int sd, rc, max_sd, i, fd;
    struct sockaddr_in addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("can't socket():");
        return -1;
    }

    addr.sin_family = AF_INET;

    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        perror("can't bind(): \n");
        return -1;
    }

    rc = listen(sd, 10);
    if (rc != 0)
    {
        perror("can't listen():");
        return -1;
    }

    printf("Ready for clients to connect(). Listening on port %d.\n", SOCK_PORT);
    do
    {
        max_sd = sd;

        fd_set set;
        FD_ZERO(&set);
        FD_SET(sd, &set);

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] > 0)
                FD_SET(clients[i], &set);

            max_sd = (clients[i] > max_sd) ? (clients[i]) : (max_sd);
        }

        struct timeval timeout = {15, 0}; // 15 sec
        rc = select(max_sd + 1, &set, NULL, NULL, &timeout);
        if (rc == 0)
        {
            printf("\nServer closed connection by timeout.\n\n");
            close(sd);
            return 0;
        }

        else if (rc == -1)
        {
            perror("can't select():");
            return EXIT_FAILURE;
        }

        if (FD_ISSET(sd, &set))
            connection_handler(sd);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            fd = clients[i];
            if ((fd > 0) && FD_ISSET(fd, &set))
                client_handler(fd, i);
        }
    } while (1);

    return 0;
}