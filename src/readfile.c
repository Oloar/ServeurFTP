/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

void echo(int connfd)
{
    int fd, code;
    int size, sizeSum;
    char buf[MAXLINE],
         fname[MAXLINE];
    memset(fname, 0, MAXLINE);
// TODO: Move everything but file reading to other function.
    // Wait for the command code.
    recv(connfd, &code, sizeof(int), 0);
    printf("Received code %i.\n", code);

    // if code valid send confirmation
    strcpy(buf, "ACK");
    send(connfd, buf, strlen(buf), 0);
    printf("ACK\n");

    printf("Waiting for filename\n");
    recv(connfd, fname, MAXLINE, 0);
    printf("Received filename %s\n", fname);
// end of TOD0

// Keep below in function
    fd = open(fname, O_RDONLY);
    if(fd != -1) {
        memset(buf, 0, MAXLINE);
        sizeSum = 0;
        while((size = read(fd, buf, MAXLINE)) != 0) {
            // send code transmission in progress
            // send information about part of file
            // printf("read %d Bytes\n", size);
            send(connfd, buf, size, 0);
            sizeSum += size;
        }
        printf("Transfer complete. %d Bytes sent.\n", sizeSum);
    }
    close(fd);
}
