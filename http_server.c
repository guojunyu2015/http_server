#include "pub_include.h"

#define SERV_PORT 8888 

int main()
{
    struct sockaddr_in stServer_addr,stClient_addr;
    int ret = 0;
    int iSockfd = 0;
    int iCli_sockfd = 0,iLen = 0;
    char aIp_addr[64+1];
    int pid,i;
    char aLog_filename[64];
    struct threadpool *thread_pool;
    pthread_t new_thread;
    int *tmp = NULL;
    
    memset(aLog_filename,0x00,sizeof(aLog_filename));
    strcpy(aLog_filename,"main_thread");
    
    /*解析全局结构体*/
    nAnalyseCfgFilePubDeal("DEBUG_LEVEL",sgTransConf.aDebug_level);
    nAnalyseCfgFilePubDeal("DEBUG_FLAG",sgTransConf.aDebug_flag);
    nAnalyseCfgFilePubDeal("THREAD_POLL_NUM",sgTransConf.aThread_pool_num);
    sgTransConf.iThread_pool_num = atoi(sgTransConf.aThread_pool_num);
    nAnalyseCfgFilePubDeal("JOB_QUEUE_MAX_NUM",sgTransConf.aQueue_job_num);
    sgTransConf.iQueue_job_num = atoi(sgTransConf.aQueue_job_num);
    printf("配置文件中日志打印级别为:%s,日志调试标志:%s,线程池最大数量:%d,任务队列最大任务数:%d\n",
            sgTransConf.aDebug_level,
            sgTransConf.aDebug_flag,
            sgTransConf.iThread_pool_num,
            sgTransConf.iQueue_job_num);
    
    iSockfd = socket(AF_INET,SOCK_STREAM,0);
    if(iSockfd == -1)
    {
        Debug(3,aLog_filename,"创建socket句柄失败,失败原因[%s]",strerror(errno));
        return -1;
    }
    Debug(3,aLog_filename,"文件描述符:[%d],服务端进程号:[%d]",iSockfd,getpid());
    
    memset(&stServer_addr,0x00,sizeof(stServer_addr));
    memset(&stClient_addr,0x00,sizeof(stClient_addr));
    memset(aIp_addr,0x00,sizeof(aIp_addr));
    
    /*绑定端口*/
    stServer_addr.sin_port = htons(SERV_PORT);
    stServer_addr.sin_addr.s_addr = htons(INADDR_ANY);
    ret = bind(iSockfd,(struct sockaddr*)&stServer_addr,sizeof(stServer_addr));
    if(ret == -1)
    {
        Debug(3,aLog_filename,"绑定端口失败,失败原因:[%s]",strerror(errno));
        return -1;
    }

    ret = listen(iSockfd,5);
    if(ret == -1)
    {
        Debug(3,aLog_filename,"开启监听失败,失败原因[%s]",strerror(errno));
        return -1;
    }
    
    /*初始化线程池*/
    thread_pool = threadpool_init(sgTransConf.iThread_pool_num,sgTransConf.iQueue_job_num);
    if(thread_pool == NULL)
    {
        Debug(3,aLog_filename,"创建线程池失败");
        return -1;
    }
    
    tmp = (int *)malloc(sizeof(int));
    
    while(1)
    {
        memset(&stClient_addr,0x00,sizeof(stClient_addr));
        iCli_sockfd = 0;
        
        iLen = sizeof(stClient_addr);
        iCli_sockfd = accept(iSockfd,(struct sockaddr*)&stClient_addr,&iLen);
        if(iCli_sockfd == -1)
        {
            Debug(3,aLog_filename,"接收失败,iSockfd = [%d],失败原因[%s],进程号[%d]",iSockfd,strerror(errno),getpid());
            return -1;
        }
        *tmp = iCli_sockfd;
        inet_ntop(AF_INET,&stClient_addr.sin_addr.s_addr,aIp_addr,sizeof(aIp_addr)-1);
        Debug(3,aLog_filename,"接收到请求,IP:[%s],端口号:[%d],请求报文长度:[%d]",
                aIp_addr,ntohs(stClient_addr.sin_port),iLen);
        
        /*将请求处理添加到任务列表中,等待线程池处理*/
        ret = threadpool_add_job(thread_pool,nHttpInfoDeal,tmp);
    }
    
    free(tmp);
    return 0;
}
