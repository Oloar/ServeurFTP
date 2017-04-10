#ifndef __FTP_H__
#define __FTP_H__


#include "csapp.h"


void send_code(int c, int sockfd);
int recv_code(int sockfd);

void ftp(int connfd);


#endif
