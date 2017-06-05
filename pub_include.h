#ifndef _PUB_INCLUDE_H_
#define _PUB_INCLUDE_H_

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
#include <pthread.h>
#include <stdarg.h>

/*HTTP请求报文首部信息*/
/*
	<method> <request-URL> <version>
	<headers>
	
	<entity-body>
*/
typedef struct http_head_info
{
	char aMethod[6+1];		/*方法*/
	char aRequest_url[64];	/*请求URL*/
	char aVersion[32];		/*版本号*/
	int  iBody_len;			/*报文体长度,根据首部中的Content-type(类型),Content-length(长度)来判断*/
}ReqHeadMsg;

/*HTTP应答报文首部信息*/
/*
	<version> <status> <reason-phrase>
	<headers>
	
	<entity-body>
*/
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

void sig_chld( int signo);
int nAnalyseCfgFilePubDeal(char *aConfig_desc,char *aConfig_str);
void * nHttpInfoDeal(void  *t);
int nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info);
int nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info);
void bsWPubDebug(int debug_level,char *aLog_file_name,char *fmt,...);

/*交易全局结构体,该结构体内容对应http_server.conf配置文件中的配置信息*/
struct tran_conf
{
	char aDebug_level[2+1];
}sgTransConf;

#endif