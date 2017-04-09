/*
 * echoclient.c - An echo client
 */
#include <time.h>
#include "csapp.h"
#include "parse.h"

int read_command(char *buf);

int main(int argc, char **argv) {
    int fd,
        size,
        port,
        nbWord,
        nbBytes,
        cmdCode,
        clientfd,
        nbBytesSum;
    char *host,
         buf[MAXLINE];

    char **cmd;

    clock_t time,
            endTime,
            beginTime;


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
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");
    printf("PID: %d\n", getpid());
    
    // Rio_readinitb(&rio, clientfd);
    while(1) {
        if (read_command(buf)) { // lecture commande
            buf[strlen(buf)] = '\0';
            cmd = split_in_words(buf);
            nbWord = nb_words(cmd);
            cmdCode = command_code(cmd[0]);

            switch(cmdCode) {
                case GET:
                    printf("GET\n");
                    send(clientfd, &cmdCode, sizeof(int), 0); // Send command
                    recv(clientfd, buf, MAXLINE, 0);
                    if (strncmp(buf, "ACK", 3) != 0) {
                        printf("Err: Server did not acknowledge command.\n");
                    }
                    else {
                        for (int i = 1; i < nbWord; ++i) { // go through files names
                            // ftp_get(cmd[i], clientfd, &rio); // get a file

                            printf("Sending filename %s\n", cmd[i]);
                            size = strlen(cmd[i]);

                            cmd[i] = realloc(cmd[i], sizeof(char) * (size + 1));
                            cmd[i][size] = '\0';

                            send(clientfd, cmd[i], strlen(cmd[i]), 0);
                            // IDEA: Send permission before hand and copy those localy
                            if((fd = open(cmd[i], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)) == -1) {
                                perror("Could not open the file: ");
                            }
                            else {
                                beginTime = clock();
                                nbBytesSum = 0;
                                while ((nbBytes = recv(clientfd, buf, MAXLINE, 0)) > 0) {
                                        write(fd, buf, nbBytes);
                                        nbBytesSum += nbBytes;
                                }
                                endTime = clock();
                                time = (endTime - beginTime) / CLOCKS_PER_SEC;
                                printf("%i bytes received in %lu seconds\n", nbBytesSum, time);
                            }
                        }
                    }
                    break;
                case PUT:
                    printf("PUT\n");
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
    Close(clientfd);
    exit(0);
}


int read_command(char *buf) {
    printf("ftp> ");
    return (Fgets(buf, MAXLINE, stdin) != NULL);
}