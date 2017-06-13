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
void *nHttpInfoDeal(void  *t);
int nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info);
int nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info);
void bsWPubDebug(const char *func_name,const int file_line,int debug_level,char *aLog_file_name,char *fmt,...);

#define Debug(debug_level,file_name,...) bsWPubDebug(__FUNCTION__,__LINE__,debug_level,file_name,##__VA_ARGS__)

/*交易全局结构体,该结构体内容对应http_server.conf配置文件中的配置信息*/
struct tran_conf
{
	char aDebug_level[2+1];
	char aDebug_flag[6+2];
	char aThread_pool_num[8+1];
	int iThread_pool_num;
	char aQueue_job_num[8+1];
	int iQueue_job_num;
}sgTransConf;

/*线程池定义相关结构体*/

/*任务队列节点,线程池中所有运行和等待的都是一个job,任务队列为链表实现的队列*/
struct job
{
	void* (*callback_func)(void *arg);	/*线程回调函数*/
	void *arg;      /*回调函数的参数*/
	struct job *next;
};

/*线程池结构*/
struct threadpool
{
	int thread_max_num;	    /*线程池开启的线程个数,该数目配置在配置文件中*/
	int queue_max_num;		/*任务队列最大存储任务的个数,该数目配置在配置文件中*/
	
	struct job *head;		/*任务队列的头指针*/
	struct job *tail;		/*任务队列的尾指针*/
	int cur_job_num;        /*任务队列当前任务数*/
	
	pthread_t *pthreads;    /*线程池中所有的线程编号*/
	
	int shutdown;			/*线程池销毁标志 0-未销毁 1-已销毁,该变量用于放置重复调用销毁处理*/
	
	pthread_mutex_t queue_lock;     /*任务队列互斥锁*/
	pthread_cond_t queue_not_empty; /*任务队列非空,线程处理函数*/
	pthread_cond_t queue_full;      /*任务队列已满*/
};

struct threadpool* threadpool_init(int thread_num, int queue_max_num);
int threadpool_add_job(struct threadpool *pool,void*(*callback_function)(void *arg),void *arg);
void *threadpool_function(void *arg);
int threadpool_destroy(struct threadpool *pool);

#endif