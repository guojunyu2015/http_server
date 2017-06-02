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

/*HTTP�������ײ���Ϣ*/
/*
	<method> <request-URL> <version>
	<headers>
	
	<entity-body>
*/
typedef struct http_head_info
{
	char aMethod[6+1];		/*����*/
	char aRequest_url[64];	/*����URL*/
	char aVersion[32];		/*�汾��*/
	int  iBody_len;			/*�����峤��,�����ײ��е�Content-type(����),Content-length(����)���ж�*/
}ReqHeadMsg;

/*HTTPӦ�����ײ���Ϣ*/
/*
	<version> <status> <reason-phrase>
	<headers>
	
	<entity-body>
*/
typedef struct rsp_http_msg
{	
	char aVersion[32];		/*�汾*/
	char aStatus[8+1];		/*״̬*/
	char aReason[64];
	char aHeader[64];
	char aEntry[1024];
}RspHttpMsg;

typedef struct head_line_info
{
	char aTitle[256];	/*����*/
	char aMsg[1024];	/*����*/
}HeadLineInfo;

void sig_chld( int signo);
int nAnalyseCfgFilePubDeal(char *aConfig_desc,char *aConfig_str);
int nHttpInfoDeal(int iSock_fd);
int nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info);
int nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info);

/*����ȫ�ֽṹ��,�ýṹ�����ݶ�Ӧhttp_server.conf�����ļ��е�������Ϣ*/
struct tran_conf
{
	int debug_level;		/*��־��ӡ����*/
}sgTransConf;

#endif