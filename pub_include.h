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
void * nHttpInfoDeal(void  *t);
int nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info);
int nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info);
void bsWPubDebug(const char *func_name,const int file_line,int debug_level,char *aLog_file_name,char *fmt,...);

#define Debug(debug_level,file_name,...) bsWPubDebug(__FUNCTION__,__LINE__,debug_level,file_name,##__VA_ARGS__)
/*����ȫ�ֽṹ��,�ýṹ�����ݶ�Ӧhttp_server.conf�����ļ��е�������Ϣ*/
struct tran_conf
{
	char aDebug_level[2+1];
	char aDebug_flag[6+2];
}sgTransConf;

/*�̳߳ض�����ؽṹ��*/
struct job
{
	void* (callback_function)(void *arg);	/*�̻߳ص�����*/
	void *arg;
	struct job *next;
}

struct threadpool
{
	int thread_num;			/*�̳߳ؿ������̸߳���*/
	int queue_max_num;		/*�����������������*/
	struct job *head;		/*�������е�ͷָ��*/
	struct job *tail;		/*�������е�βָ��*/
	pthread_t *pthreads;    /*�̳߳������е��̱߳��*/
	pthread_mutex_t mutex;  /*�����ź���*/
	pthread_cond_t  queue_empty;    /*����Ϊ�յ���������*/
	pthread_cond_t  queue_not_empty;/*���в�Ϊ�յ���������*/
	pthread_cond_t  queue_not full; /*���в�������������*/
	int queue_cur_num;      /*���е�ǰ��job����*/
	int queue_close;        /*�����Ƿ��ѹر�*/
	int pool_close;         /*�̳߳��Ƿ��ѹر�*/
};


#endif