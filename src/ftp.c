/*
 * echo - read and echo text lines until client closes connection
 */
#include <dirent.h> 
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

void acknowledge(int sockfd, char *msg) {
    send(sockfd, msg, strlen(msg), 0);
}

int wait_ack(int sockfd, char *msg) {
    char buf[BLOCKSIZE];

    recv(sockfd, buf, BLOCKSIZE, 0);
    return (strncmp(buf, msg, strlen(msg)) != 0);
}

// REQUESTS

int get_request(char *fname, int sockfd) {
    int fd, size, transfered;

    char buf[BLOCKSIZE];

    clock_t end,
            time,
            begin;

    // Send command
    send_code(GET, sockfd); 
    // Wait for confirmation
    if (wait_ack(sockfd, "ACK")) {
        printf("Err: Server did not acknowledge command.\n");
        return 1;
    }
    else {
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
            while ((size = recv(sockfd, buf, BLOCKSIZE, 0)) > 0) {
                    write(fd, buf, size);
                    transfered += size;
            }
            end = clock();
            time = (end - begin) / CLOCKS_PER_SEC;
            printf("Transfer succesfully completed.\n");
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
    if (wait_ack(sockfd, "ACK")) {
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
            memset(buf, 0, BLOCKSIZE);
            begin = clock();
            transfered = 0;
            while((size = read(fd, buf, BLOCKSIZE)) != 0) {
                // send code transmission in progress
                // send information about part of file

                // (sent = send(client, file_data + offset, nbytes, 0)) > 0
                send(sockfd, buf, size, 0);
                transfered += size;
            }
            end = clock();
            time = (end - begin) / CLOCKS_PER_SEC;
            printf("Transfer succesfully completed.\n");
            printf("%i bytes received in %lu seconds\n", transfered, time);
        }
    }
    return 0;
}

int sh_request(int sockfd, int code, char *path) {
    int size;
    char buf[BLOCKSIZE];

    if (code == CD) {
        // TODO
        printf("Not implemented.\n");
        return 1;

        // Send command
        send_code(code, sockfd);
        // Wait for confirmation
        if (wait_ack(sockfd, "ACK")) {
            printf("Err: Server did not acknowledge command.\n");
            return 1;
        }

        printf("Sending path: %s\n", path);
        strcpy(buf, path);
        send(sockfd, buf, strlen(buf), 0);

        strcpy(buf, "\0");
        send(sockfd, buf, strlen(buf), 0); // send last arg = "\0"
    }
    else {
        // Send command
        send_code(code, sockfd);
        // Wait for confirmation
        if (wait_ack(sockfd, "ACK")) {
            printf("Err: Server did not acknowledge command.\n");
            return 1;
        }
        // Wait for output
        while((size = read(sockfd, buf, BLOCKSIZE)) > 0) {
            write(STDOUT_FILENO, buf, size);
        }
    }
    return 0;
}

int exec_command(int sockfd, char *cmd, char **args) {
    // Send command
    send_code(command_code(cmd), sockfd);
    if(fork() == 0) {
        dup2(sockfd, STDOUT_FILENO);
        // dup2(sockfd, STDERR_FILENO);
        close(sockfd);
        execvp(cmd, args);
        return 1;
    }
    else {
        wait(NULL);
    }
    return 0;
}

// Main ftp function
void ftp(int connfd) {
    int fd, code;
    int size, transfered;
    char buf[BLOCKSIZE],
         fname[BLOCKSIZE],
         *args[]  = {"\0"};

    memset(fname, 0, BLOCKSIZE);

    // Wait for the command code.
    code = recv_code(connfd);
    printf("Received code %i.\n", code);

    if(code == -1) { // if an incorrect command passed through.
        printf("Command unknown.\n");
        return;
    }
    // if code valid send confirmation
    acknowledge(connfd, "ACK");
    printf("ACK\n");
    switch(code) {
        case GET:
          // Below in function send_file            
            printf("Waiting for filename\n");
            recv(connfd, fname, BLOCKSIZE, 0);
            printf("Received filename %s\n", fname);

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
            printf("Waiting for filename\n");
            recv(connfd, fname, BLOCKSIZE, 0);
            printf("Received filename %s\n", fname);
            
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
            printf("Client exited.\n");
            break;
        case LS:
            exec_command(connfd, "ls", args);
            break;
        case PWD:
            exec_command(connfd, "pwd", args);
            break;
        case CD:
            printf("CD not implemented\n");
            //  FIXME: ACK not received.

            // while((size = recv(connfd, buf, BLOCKSIZE, 0)) > 0) {
            //     printf("read %i bytes. buf: %s\n", size, buf);
            //     if(strncmp(buf,"\0", 1) == 0) {
            //         printf("NO ARGS\n");
            //     }
            // }

            // TODO: Use chdir() to change directory.
            break;
        default: ;
    }
}