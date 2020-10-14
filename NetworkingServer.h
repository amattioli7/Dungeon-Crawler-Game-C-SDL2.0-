
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>
#include <sys/signalfd.h>

#include <poll.h>

#define BUF_SIZE    1024     

typedef struct addr_list_t
{
        struct sockaddr_in addr;
        struct addr_list_t* next;
} addr_list_t;

void EchoMessage(const int socketFd, const char *message, addr_list_t *list);
int DoEcho(const int socketFd);

int CompairSockAddr(const struct sockaddr_in *s1, const struct sockaddr_in *s2);

int AddAddr(const struct sockaddr_in *addr, addr_list_t **list);
int RemoveAddr(const struct sockaddr_in *addr, addr_list_t **list);


int main(int argc, char *argv[])
{
        int result;
        int socketFd;           /* server's socket descriptor */

        /* structure for echo server internet addresses */
        struct sockaddr_in serverAddr;

        /* argv[1] is port number, make sure it's passed to us */
        if (argc != 2)
        {
                fprintf(stderr, "Usage:  %s <port number>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* create a socket file descriptor for upd data */
        socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (socketFd < 0)
        {
                perror("Error creating socket");
                exit(EXIT_FAILURE);
        }

        /* allow internet data from any address on port argv[1] */
        memset(&serverAddr, 0, sizeof(serverAddr)); /* clear data structure */
        serverAddr.sin_family = AF_INET;            /* internet addr family */
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* any incoming address */
        serverAddr.sin_port = htons(atoi(argv[1])); /* port number */

        /* bind the socket to the local address */
        result = bind(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        if (result < 0)
        {
                /* bind failed */
                perror("Error binding socket");
                close(socketFd);
                exit(EXIT_FAILURE);
        }

        /* we have a good socket bound to a port, echo all received packets */
        DoEcho(socketFd);

        close(socketFd);

        if (result < 0)
        {
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}



void EchoMessage(const int socketFd, const char *message, addr_list_t *list)
{
        int result;
        addr_list_t *here;

        here = list;

        while (here != NULL)
        {
                result = sendto(socketFd, message, strlen(message), MSG_DONTWAIT,
                                (struct sockaddr *)&(here->addr), sizeof(struct sockaddr_in));

                if (result == -1)
                {
                        if ((EAGAIN == errno) || (EWOULDBLOCK == errno))
                        {
                                fprintf(stderr, "Socket is busy\n");
                        }
                        else
                        {
                                /* send failed */
                                //perror("Error echoing message");
                        }
                }

                here = here->next;
        }
}

int DoEcho(const int socketFd)
{
        struct sockaddr_in clientAddr;  /* address that sent the packet */
        char buffer[BUF_SIZE + 1];      /* stores received message */
        int result;

        /* we'll need these to handle ctrl-c, ctrl-\ while trying to recv */
        sigset_t mask, oldMask;
        int signalFd;

        struct pollfd pfds[2];  /* poll for socket recv and signal */
        addr_list_t *addrList;

        /* mask ctrl-c and ctrl-\ */
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        sigaddset(&mask, SIGQUIT);

        /* block ctrl-c and ctrl-\ signals so that can be signaled
         * signaled by the signalFd and handled while polling. */
        if (sigprocmask(SIG_BLOCK, &mask, &oldMask) == -1)
        {
                perror("Error setting sigproc mask");
                return 0;
        }

        signalFd = signalfd(-1, &mask, 0);

        if (signalFd == -1)
        {
                perror("Error creating signal fd");
                sigprocmask(SIG_UNBLOCK, &mask, NULL);
                sigprocmask(SIG_BLOCK, &oldMask, NULL);
                return 0;
        }

        pfds[0].fd = socketFd;
        pfds[0].events = POLLIN;

        pfds[1].fd = signalFd;
        pfds[1].events = POLLIN;

        addrList = NULL;

        while (1)
        {
                /* start with cleared buffer and client address structure */
                memset(&buffer, 0, BUF_SIZE);
                memset(&clientAddr, 0, sizeof(struct sockaddr_in));

                printf("Waiting to receive a message [ctrl-c exits]:\n");
                result = 0;

                poll(pfds, 2, -1); /* block with poll until 1 of 2 events */

                /* handle signals first */
                if (pfds[1].revents & POLLIN)
                {
                        /* SIGINT or SIGQUIT get out of here */
                        break;
                }

                /* now check for recvfrom on socket */
                if (pfds[0].revents & POLLIN)
                {
                        socklen_t addrLen;

                        /* use recvfrom so we can know where the data came from */
                        result = recvfrom(socketFd, buffer, BUF_SIZE, 0,
                                          (struct sockaddr *)&clientAddr, &addrLen);

                        if (result < 0)
                        {
                                perror("Error receiving message");
                        }
                        else
                        {
                                /* we received a valid message */
                                char from[INET_ADDRSTRLEN + 1];
                                from[0] = '\0';

                                if (NULL !=
                                    inet_ntop(AF_INET, (void *)&(clientAddr.sin_addr), from,
                                              INET_ADDRSTRLEN))
                                {
                                        printf("Received message from %s:%d: ", from,
                                               ntohs(clientAddr.sin_port));
                                }
                                else
                                {
                                        printf("Received message from unresolveble address\n");
                                }

                                if (strlen(buffer) > 0)
                                {
                                        printf("%s\n", buffer);
                                        AddAddr(&clientAddr, &addrList);

                                        /* now try echoing the buffer to all addresses */
                                        EchoMessage(socketFd, buffer, addrList);
                                }
                                else
                                {
                                        printf("Message was empty\n");
                                        RemoveAddr(&clientAddr, &addrList);
                                }
                        }
                }
        }

        /* we should only get here by SIGINT or SIGQUIT. clean-up signal mask */
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        sigprocmask(SIG_BLOCK, &oldMask, NULL);
        close(signalFd);
        return result;
}


int CompairSockAddr(const struct sockaddr_in *s1, const struct sockaddr_in *s2)
{
        return memcmp(s1, s2, sizeof(struct sockaddr_in));
}


int AddAddr(const struct sockaddr_in *addr, addr_list_t **list)
{
        addr_list_t *here;

        /* handle empty list */
        if (NULL == *list)
        {
                *list = (addr_list_t *)malloc(sizeof(struct sockaddr_in));

                if (NULL == list)
                {
                        perror("Error allocating addr_list_t");
                        return errno;
                }

                memcpy(&((*list)->addr), addr, sizeof(struct sockaddr_in));
                (*list)->next = NULL;
                return 0;
        }

        /* find the end of the list checking to see if address is in the list */
        here = *list;
        while(here->next != NULL)
        {
                if (CompairSockAddr(&(here->addr), addr) == 0)
                {
                        /* the address is alread in the list */
                        return 0;
                }

                here = here->next;
        }

        /* we're at the end of the list make sure the address isn't here */
        if (CompairSockAddr(&(here->addr), addr) == 0)
        {
                /* the address is alread in the list */
                return 0;
        }

        /* the address in new, add it to the end of the list */
        here->next = (addr_list_t *)malloc(sizeof(addr_list_t));

        if (NULL == here->next)
        {
                perror("Error allocating addr_list_t");
                return errno;
        }

        memcpy(&(here->next->addr), addr, sizeof(struct sockaddr_in));
        here->next->next = NULL;
        return 0;
}

int RemoveAddr(const struct sockaddr_in *addr, addr_list_t **list)
{
        addr_list_t *here, *prev;

        if (NULL == *list)
        {
                return 0;
        }

        here = *list;
        prev = NULL;

        while (here != NULL)
        {
                if (CompairSockAddr(&(here->addr), addr) == 0)
                {
                        /* found it */
                        if (NULL == prev)
                        {
                                /* deleted the head */
                                *list = here->next;
                        }
                        else
                        {
                                prev->next = here->next;
                        }

                        free(here);
                        return 0;
                }

                prev = here;
                here = here->next;
        }

        /* client will not be in the list if it only sends an empty message */
        return 0;
}
