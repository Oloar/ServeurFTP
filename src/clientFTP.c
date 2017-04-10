/*
 * echoclient.c - An echo client
 */
#include <time.h>
#include "csapp.h"
#include "parse.h"
#include "ftp.h"

// int read_command(char *buf);
// int get_request(char *fname, int sockfd);
// int put_request(char *fname, int sockfd);


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
            buf[strlen(buf)-1] = '\0';
            cmd = split_in_words(buf);
            nbWord = nb_words(cmd);
            cmdCode = command_code(cmd[0]);
            // printf("cmd: %s -> %d\n", cmd[0], cmdCode);
            switch(cmdCode) {
                case QUIT:
                    printf("Exit.\n");
                    clientfd = Open_clientfd(host, port);
                    send_code(QUIT, clientfd); 
                    close(clientfd);
                    exit(0);
                break;
                case LS:
                case PWD:
                    clientfd = Open_clientfd(host, port);
                    sh_request(clientfd, cmdCode, "\0");
                break;
                case CD:
                    if (nbWord != 2 ) {
                        printf("cd require a single argument.\n");
                    }
                    else {
                        sh_request(clientfd, cmdCode, cmd[1]);
                    }
                break;
                case -1 : 
                    printf("Err: commande non reconnue.\n");
                break;
                default: 
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
                        }
                    }
                break;
            }
        }
    }
    return 0;
}


int read_command(char *buf) {
    printf("ftp> ");
    return (Fgets(buf, MAXLINE, stdin) != NULL);
}
