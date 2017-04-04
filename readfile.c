/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

void echo(int connfd)
{
    int fd;
    rio_t rio;
    size_t size, idx;
    char buf[MAXLINE],
         fname[MAXLINE];

    Rio_readinitb(&rio, connfd);
    idx =  Rio_readlineb(&rio, fname, MAXLINE);
    if(idx > 1) {
        if (fname[idx-1] == '\n') {
            fname[idx-1] = '\0';
            idx--;
        }
    }
    fd = open(fname, O_RDONLY);
    if(fd != -1) {
        while((size = read(fd, buf, MAXLINE)) != 0) {
            printf("read %lu Bytes\n", size);
            Rio_writen(connfd, buf, size);
        }
    }
    close(fd);
}
