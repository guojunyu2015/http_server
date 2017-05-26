#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>

int sock_writen(int iSock_fd,char *aStr_tmp,int iLen);
int sock_readn(int iSock_fd,char *aStr_tmp,int iLen);
void sig_chld( int signo);