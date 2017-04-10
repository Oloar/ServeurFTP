/*
 * echoserveri.c - An iterative echo server
 */       
 #include <sys/types.h>
 #include <sys/wait.h>
#include <signal.h>

#include "csapp.h"
#include "ftp.h"


#define MAX_NAME_LEN 256
#define NBPROC 2
// #define LOGFILE _log

void echo(int connfd);


void handler_SIGCHLD(int sig) {
    while(wait(NULL) >0);
    return;
}
void handler_SIGINT(int sig) {
    kill(0, SIGINT);
    exit(0);
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv) {
    int listenfd, connfd, port, status;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    pid_t pid, 
        father = getpid(),
        proc_pool[NBPROC];

    signal(SIGCHLD, handler_SIGCHLD);
    signal(SIGINT, handler_SIGINT);

    port = 2121; // Config par défaut

    clientlen = (socklen_t)sizeof(clientaddr);


    listenfd = Open_listenfd(port);

    for (int i = 0; i < NBPROC; ++i) {
        if (getpid() == father) {
            pid = fork();
            proc_pool[i] = pid;

        }
    }
    if(pid == 0){
        while (1) {
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAX_NAME_LEN, 0, 0, 0);
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string, INET_ADDRSTRLEN);
                printf("Client connected to %s (%s). Server on PID: %d\n", client_hostname, client_ip_string, getpid());

                ftp(connfd);
                close(connfd);
        }
    }
    else {
        for (int i = 0; i < NBPROC; ++i){
            wait(&status);
        }
    }
    exit(0);
}
