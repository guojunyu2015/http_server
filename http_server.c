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
	
	signal(SIGCHLD,&sig_chld);
	iSockfd = socket(AF_INET,SOCK_STREAM,0);
	if(iSockfd == -1)
	{
		printf("创建socket句柄失败,失败原因[%s]\n",strerror(errno));
		return -1;
	}
	printf("文件描述符:[%d],服务端进程号:[%d]\n",iSockfd,getpid());
	memset(&stServer_addr,0x00,sizeof(stServer_addr));
	memset(&stClient_addr,0x00,sizeof(stClient_addr));
	memset(aIp_addr,0x00,sizeof(aIp_addr));
	
	/*绑定端口*/
	stServer_addr.sin_port = htons(SERV_PORT);
	stServer_addr.sin_addr.s_addr = htons(INADDR_ANY);
	ret = bind(iSockfd,(struct sockaddr*)&stServer_addr,sizeof(stServer_addr));
	if(ret == -1)
	{
		printf("绑定端口失败,失败原因:[%s]\n",strerror(errno));
		return -1;
	}

	ret = listen(iSockfd,5);
	if(ret == -1)
	{
		printf("开启监听失败,失败原因[%s]\n",strerror(errno));
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
			printf("接收失败,iSockfd = [%d],失败原因[%s],进程号[%d]\n",iSockfd,strerror(errno),getpid());
			return -1;
		}
		inet_ntop(AF_INET,&stClient_addr.sin_addr.s_addr,aIp_addr,sizeof(aIp_addr)-1);
		printf("\n接收到请求,IP:[%s],端口号:[%d],请求报文长度:[%d]\n",
				aIp_addr,ntohs(stClient_addr.sin_port),iLen);
		if((pid = fork()) == 0)		/*如果为子进程*/
		{
			close(iSockfd);
			ret = nAnalyzeHttpRequestInfo(iCli_sockfd);
			if(ret)
			{
				printf("解析HTTP报文失败,ret = %d\n",ret);
				return -1;
			}
			send(iCli_sockfd,"服务器已接收并成功处理请求",strlen("服务器已接收并成功处理请求"),0);
			close(iCli_sockfd);
			exit(0);	
		}
		
		close(iCli_sockfd);
	}
	
	return 0;
}
