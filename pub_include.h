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

void sig_chld( int signo);
int nAnalyzeHttpRequestInfo(int iSock_fd);

/*HTTP请求报文首部信息*/
typedef struct http_head_info
{
	char aMethod[6+1];		/*方法*/
	char aRequest_url[64];	/*请求URL*/
	char aVersion[32];		/*版本号*/
	int  iBody_len;			/*报文体长度,根据首部中的Content-type(类型),Content-length(长度)来判断*/
}ReqHeadMsg;

/*HTTP应答报文首部信息*/
typedef struct rsp_http_msg
{	
	char aVersion[32];		/*版本*/
	char aStatus[8+1];		/*状态*/
	char aReason[64];
	char aHeader[64];
	char aEntry[1024];
}RspHttpMsg;

typedef struct head_line_info
{
	char aTitle[256];	/*标题*/
	char aMsg[1024];	/*内容*/
}HeadLineInfo;