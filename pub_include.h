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
int nAnalyzeHttpRequestInfo(char *aRcv_msg,int iMsg_len);

/*HTTP��������ϸ*/
typedef struct req_http_msg
{
	char aMethod[6+1];		/*����*/
	char aRequest_url[64];	/*����URL*/
	char aVersion[32];		/*�汾��*/
	char aHeaders[256];		/*�ײ���*/
	char aEntry_body[1024];	/*����*/
}ReqHttpMsg;

/*HTTPӦ������ϸ*/
typedef struct rsp_http_msg
{	
	char aVersion[32];		/*�汾*/
	char aStatus[8+1];		/*״̬*/
	char aReason[64];
	char aHeader[64];
	char aEntry[1024];
}RspHttpMsg;
