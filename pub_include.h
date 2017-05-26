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

/*HTTP请求报文明细*/
typedef struct req_http_msg
{
	char aMethod[6+1];		/*方法*/
	char aRequest_url[64];	/*请求URL*/
	char aVersion[32];		/*版本号*/
	char aHeaders[256];		/*首部块*/
	char aEntry_body[1024];	/*主体*/
}ReqHttpMsg;

/*HTTP应答报文明细*/
typedef struct rsp_http_msg
{	
	char aVersion[32];		/*版本*/
	char aStatus[8+1];		/*状态*/
	char aReason[64];
	char aHeader[64];
	char aEntry[1024];
}RspHttpMsg;
