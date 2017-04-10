/*
 * echoclient.c - An echo client
 */
#include <time.h>
#include "csapp.h"
#include "parse.h"
#include "ftp.h"

int read_command(char *buf);
int get_request(char *fname, int sockfd);
int put_request(char *fname, int sockfd);

int main(int argc, char **argv) {
    int port,
        nbWord,
        cmdCode,
        clientfd;

    char *host,
         buf[MAXLINE];

    char **cmd;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    // clientfd = Open_clientfd(host, port);

    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    //printf("client connected to server. PID: %d\n", getpid());

    // Receive slave infos
    // recv(clientfd, buff, BLOCKSIZE, 0); // reveive slave address


    // Rio_readinitb(&rio, clientfd);
    while(1) {
        if (read_command(buf)) { // lecture commande
            buf[strlen(buf)] = '\0';
            cmd = split_in_words(buf);
            nbWord = nb_words(cmd);
            cmdCode = command_code(cmd[0]);
            for (int i = 1; i < nbWord; ++i) {
                // TODO: (Loadbalance) Connect to master and request access to slave
                // Moved to prepare connection to slave
                clientfd = Open_clientfd(host, port); // change host to slave
                // printf("client connected to server. PID: %d\n", getpid()); // connected to slave

                switch(cmdCode) {
                    case GET:
                        get_request(cmd[i], clientfd);
                        break;
                    case PUT:
                        // printf("PUT\n");
                        put_request(cmd[i], clientfd);
                        break;
                    case QUIT:
                        printf("QUIT\n");
                        close(clientfd);
                        exit(0);
                        break;
                    case -1:
                        strcpy(buf, "Err: commande non reconnue.\n");
                        send(clientfd, buf, strlen(buf), 0);
                }
            }
        }
    }
    return 0;
}


int read_command(char *buf) {
    printf("ftp> ");
    return (Fgets(buf, MAXLINE, stdin) != NULL);
}


int get_request(char *fname, int sockfd) {
    int fd, size, transfered;

    char buf[BLOCKSIZE];

    clock_t end,
            time,
            begin;

    // Send command
    send_code(GET, sockfd); 
    // Wait for confirmation
    recv(sockfd, buf, MAXLINE, 0);
    if (strncmp(buf, "ACK", 3) != 0) {
        printf("Err: Server did not acknowledge command.\n");
        return 1;
    }
    else {
        // printf("Sending filename %s\n", fname);

        size = strlen(fname);
        fname = realloc(fname, sizeof(char) * (size + 1));
        fname[size] = '\0';

        send(sockfd, fname, strlen(fname), 0);

        // IDEA: Send permission before hand and copy those localy
        if((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)) == -1) {
            perror("Could not open the file: ");
            return 1;
        }
        else {
            begin = clock();
            transfered = 0;
            while ((size = recv(sockfd, buf, MAXLINE, 0)) > 0) {
                    write(fd, buf, size);
                    transfered += size;
            }
            end = clock();
            time = (end - begin) / CLOCKS_PER_SEC;
            printf("%i bytes received in %lu seconds\n", transfered, time);
        }
    }
    return 0;
}

int put_request(char *fname, int sockfd) {
    int fd, size, transfered;

    char buf[BLOCKSIZE];

    clock_t end,
            time,
            begin;

    // Send command
    send_code(PUT, sockfd); 
    // Wait for confirmation
    recv(sockfd, buf, MAXLINE, 0);
    if (strncmp(buf, "ACK", 3) != 0) {
        printf("Err: Server did not acknowledge command.\n");
        return 1;
    }
    else {
        size = strlen(fname);
        fname = realloc(fname, sizeof(char) * (size + 1));
        fname[size] = '\0';

        send(sockfd, fname, strlen(fname), 0);

        if((fd = open(fname, O_RDONLY)) == -1) {
            perror("Could not open the file: ");
            return 1;
        }
        else {
            memset(buf, 0, MAXLINE);
            begin = clock();
            transfered = 0;
            while((size = read(fd, buf, MAXLINE)) != 0) {
                // send code transmission in progress
                // send information about part of file
                send(sockfd, buf, size, 0);
                transfered += size;
            }
            end = clock();
            time = (end - begin) / CLOCKS_PER_SEC;
            printf("%i bytes received in %lu seconds\n", transfered, time);
        }
    }
    return 0;
}

/*

                case PUT:
                    // for (int i = 1; i < nbWord; ++i) { // go through files names
                        printf("PUT\n");
                        send_code(cmdCode, clientfd); // Send command
                        recv(clientfd, buf, BLOCKSIZE, 0);
                        if (strncmp(buf, "ACK", 3) != 0) {
                            printf("Err: Server did not acknowledge command.\n");
                        }
                        else {
                            printf("Sending filename %s\n", cmd[1]);
                            size = strlen(cmd[01]);

                            cmd[1] = realloc(cmd[1], sizeof(char) * (size + 1));
                            cmd[1][size] = '\0';

                            send(clientfd, cmd[1], strlen(cmd[1]), 0);
                            if((fd = open(cmd[1], O_RDONLY)) == -1) {
                                perror("Could not open the file: ");
                            }
                            else {
                                begin = clock();
                                transfered = 0;
                                while((size = read(fd, buf, BLOCKSIZE)) != 0) {
                                    // send code transmission in progress
                                    // send information about part of file
                                    send(clientfd, buf, size, 0);
                                    transfered += size;
                                }
                                end = clock();
                                time = (end - begin) / CLOCKS_PER_SEC;
                                printf("%i bytes sent in %lu seconds\n", transfered, time);
                            }
                        }
                    // }
                    break;
                case QUIT:
                    printf("QUIT\n");
                    close(clientfd);
                    exit(0);
                    break;
                case -1:
                    strcpy(buf, "Err: commande non reconnue.\n");
                    send(clientfd, buf, strlen(buf), 0);
            }


            */