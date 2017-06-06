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
void bsWPubDebug(const char *func_name,const int file_line,int debug_level,char *aLog_file_name,char *fmt,...);

#define Debug(debug_level,file_name,...) bsWPubDebug(__FUNCTION__,__LINE__,debug_level,file_name,##__VA_ARGS__)
/*交易全局结构体,该结构体内容对应http_server.conf配置文件中的配置信息*/
struct tran_conf
{
	char aDebug_level[2+1];
	char aDebug_flag[6+2];
}sgTransConf;

/*线程池定义相关结构体*/
struct job
{
	void* (callback_function)(void *arg);	/*线程回调函数*/
	void *arg;
	struct job *next;
}

struct threadpool
{
	int thread_num;			/*线程池开启的线程个数*/
	int queue_max_num;		/*队列中最大阻塞个数*/
	struct job *head;		/*阻塞队列的头指针*/
	struct job *tail;		/*阻塞队列的尾指针*/
	pthread_t *pthreads;    /*线程池中所有的线程编号*/
	pthread_mutex_t mutex;  /*互斥信号量*/
	pthread_cond_t  queue_empty;    /*队列为空的条件变量*/
	pthread_cond_t  queue_not_empty;/*队列不为空的条件变量*/
	pthread_cond_t  queue_not full; /*队列不满的条件变量*/
	int queue_cur_num;      /*队列当前的job个数*/
	int queue_close;        /*队列是否已关闭*/
	int pool_close;         /*线程池是否已关闭*/
};


#endif