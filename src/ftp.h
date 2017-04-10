#ifndef __FTP_H__
#define __FTP_H__


#include "csapp.h"


void send_code(int c, int sockfd);
int recv_code(int sockfd);

void acknowledge(int sockfd, char *msg);
int wait_ack(int sockfd, char *msg);

int read_command(char *buf);
int get_request(char *fname, int sockfd);
int put_request(char *fname, int sockfd);
int sh_request(int sockfd, int code, char *args);



void ftp(int connfd);


#endif
