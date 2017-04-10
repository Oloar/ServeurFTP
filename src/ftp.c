/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include "parse.h"
#include "ftp.h"



void send_code(int c, int sockfd) {
    int data = htonl(c);
    send(sockfd, &data, sizeof(data), 0);
}

int recv_code(int sockfd) {
    int r = 0;

    recv(sockfd, &r, sizeof(r), 0);
    return ntohl(r);
}

void ftp(int connfd) {
    int fd, code;
    int size, transfered;
    char buf[BLOCKSIZE],
         fname[BLOCKSIZE];
    memset(fname, 0, BLOCKSIZE);

    // Wait for the command code.
    code = recv_code(connfd);
    printf("Received code %i.\n", code);

    // if code valid send confirmation
    strcpy(buf, "ACK");
    send(connfd, buf, strlen(buf), 0);
    printf("ACK\n");

    printf("Waiting for filename\n");
    recv(connfd, fname, BLOCKSIZE, 0);
    printf("Received filename %s\n", fname);
    switch(code) {
        case GET:
          // Below in function send_file
            if((fd = open(fname, O_RDONLY)) == -1) {
                perror("Could not open the file: ");
            }
            else {
                memset(buf, 0, BLOCKSIZE);
                transfered = 0;
                while((size = read(fd, buf, BLOCKSIZE)) != 0) {
                    // send code transmission in progress
                    // send information about part of file
                    send(connfd, buf, size, 0);
                    transfered += size;
                }
                printf("Transfer complete. %d Bytes sent.\n", transfered);
            }
        // End of send_file
            break;
        case PUT:
        // Below in function recv_file
            if((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)) == -1) {
                perror("Could not open the file: ");
            }
            else {
                memset(buf, 0, BLOCKSIZE);
                transfered = 0;
                while((size = recv(connfd, buf, BLOCKSIZE, 0)) > 0) {
                    write(fd, buf, size);
                    transfered += size;
                }
                printf("Transfer complete. %d Bytes received.\n", transfered);
            }
        // End of recv_file
            break;
        case QUIT:
            printf("QUIT\n");
            // loop = 0;
            break;
        default: ;
    }
}