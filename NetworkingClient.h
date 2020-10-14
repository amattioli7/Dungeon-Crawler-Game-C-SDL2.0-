#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

#define BUF_SIZE    1024 
int DoEchoClient(const int socketFd, const struct sockaddr_in *serverAddr);

void NetworkingTest(){

		int result;
        int socketFd;           /* UDP socket descriptor */

        /* structures for use with getaddrinfo() */
        struct addrinfo hints;  /* hints for getaddrinfo() */
        struct addrinfo *info;  /* list of info returned by getaddrinfo() */
        struct addrinfo *p;     /* pointer for iterating list in info */

        struct sockaddr_in serverAddr; /* the address of the server for sendto */

        /* argv[1] is host name or address, argv[2] is port number,
         * make sure we have them */

        /* indicate that we want ipv4 udp datagrams */
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;      /* internet address family */
        hints.ai_socktype = SOCK_DGRAM; /* datagram sock */
        hints.ai_protocol = IPPROTO_UDP; /* udp/ip protocol */

        /* get a linked list of likely servers pointed to by info */
        result = getaddrinfo("96.236.145.9", "7790", &hints, &info);

        if (result != 0)
        {
                fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(result));
                exit(EXIT_FAILURE);
        }

        printf("Trying %s...\n");
        p = info;

        while (p != NULL)
        {
                /* use current info to create a socket */
                socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

                if (socketFd >= 0)
                {
                        /* we succeeded in getting a socket get out of this loop */
                        break;
                }

                p = p->ai_next; /* try next address */
        }

        if (NULL == p)
        {
                /* we never found a server to connect to */
                fprintf(stderr, "Unable to connect to server\n.");
                freeaddrinfo(info);
                exit(EXIT_FAILURE);
        }

        /* make a copy of the server address, so we can send messages to it */
        memset(&serverAddr, 0, sizeof(serverAddr));
        memcpy(&serverAddr, p->ai_addr, p->ai_addrlen);
        freeaddrinfo(info);

        /* send and receive echo messages until user sends empty message */
        DoEchoClient(socketFd, &serverAddr);

        close(socketFd);
}


		int DoEchoClient(const int socketFd, const struct sockaddr_in *serverAddr)
		{
		        int result;
		        char buffer[BUF_SIZE + 1]; /* stores received message */
		        struct pollfd pfds[2];  /* poll for socket recv and stdin */

		        pfds[0].fd = socketFd;
		        pfds[0].events = POLLIN;

		        pfds[1].fd = STDIN_FILENO;
		        pfds[1].events = POLLIN;

		        /* get message line from the user */
		        printf("Enter message to send [empty message exits]:\n");

		        while (1)
		        {
		                /* block with poll until user input or socket receive data */
		                poll(pfds, 2, -1);

		                /* check for recv on socket */
		                if (pfds[0].revents & POLLIN)
		                {
		                        /* get the server's reply (recv actually accepts all replies) */
		                        result = recv(socketFd, buffer, BUF_SIZE, 0);

		                        if (result < 0)
		                        {
		                                /* receiver error, print error message and exit */
		                                perror("Error receiving echo");
		                                break;
		                        }
		                        else
		                        {
		                                printf("Received bytes: %s\n", buffer);
		                                memset(buffer, 0, strlen(buffer)); /* clear buffer for reuse */
		                        }
		                }

		                /* check for message to transmit */
		                if (pfds[1].revents & POLLIN)
		                {
		                        if (NULL == fgets(buffer, BUF_SIZE, stdin))
		                        {
		                                /* error, print error message, get error code, and exit */
		                                perror("Error reading user input");
		                                result = ferror(stdin);
		                                break;
		                        }

		                        /* strip off the trailing carriage return */
		                        buffer[strlen(buffer) - 1] = '\0';

		                        /* send the message line to the server */
		                        result = sendto(socketFd, buffer, strlen(buffer) + 1, 0,
		                                        (const struct sockaddr *)serverAddr,
		                                        sizeof(struct sockaddr_in));

		                        if (result < 0)
		                        {
		                                /* error, print error message and exit */
		                                perror("Error sending message to server");
		                                break;
		                        }

		                        if (buffer[0] == '\0')
		                        {
		                                /* exit on empty message */
		                                result = 0;
		                                break;
		                        }
		                        else
		                        {
		                                /* clear the buffer and prompt for new message to echo */
		                                memset(buffer, 0, strlen(buffer));
		                                printf("Enter message to send [empty message exits]:\n");
		                        }
		                }
		        }

		        return result;
		}
