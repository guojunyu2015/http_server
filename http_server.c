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
    pthread_t new_thread;
    
    memset(aLog_filename,0x00,sizeof(aLog_filename));
    strcpy(aLog_filename,"main_thread");
    
    /*解析全局结构体*/
    nAnalyseCfgFilePubDeal("DEBUG_LEVEL",sgTransConf.aDebug_level);
    nAnalyseCfgFilePubDeal("DEBUG_FLAG",sgTransConf.aDebug_flag);
    printf("配置文件中日志打印级别为:%s,日志调试标志:%s\n",sgTransConf.aDebug_level,sgTransConf.aDebug_flag);
    
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
        inet_ntop(AF_INET,&stClient_addr.sin_addr.s_addr,aIp_addr,sizeof(aIp_addr)-1);
        Debug(3,aLog_filename,"接收到请求,IP:[%s],端口号:[%d],请求报文长度:[%d]",
                aIp_addr,ntohs(stClient_addr.sin_port),iLen);
        
        /*创建子线程执行请求处理*/
        if(pthread_create(&new_thread,NULL,nHttpInfoDeal,(void *)iCli_sockfd) != 0)
        {
            Debug(3,aLog_filename,"创建子线程失败,失败原因[%s]",strerror(errno));
        }
        Debug(3,aLog_filename,"该笔请求由子线程[%lu]执行",new_thread);
    }
    
    return 0;
}
