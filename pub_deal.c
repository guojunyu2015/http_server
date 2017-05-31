#include "pub_include.h"

/*********************************************************
 ** 函数名  :   read_line(int iSock_fd,char *aStr_tmp,int iLen)
 ** 功能    :   按指定长度读取socket描述符中的报文
 ** 全局变量:
 ** 入口参数:	iSock_fd	请求socket文件描述符
 				iLen		可以读取的最大长度
 ** 出口参数:	aStr_tmp	读取到的数据
 				
 ** 返回值:
 ***********************************************************/
int read_line(int iSock_fd,char *aStr_tmp,int iLen)
{
	/*HTTP协议采用回车换行符当做结束符,没有专门的长度域,采用回车换行符(或者单个回车符)来进行区分,逐个字符进行读取判断*/
	char c = '\0';
	int i = 0,n;
	while(i < iLen && c != '\n')
	{
		n = recv(iSock_fd,&c,1,0);
		if(n > 0)
		{
			if(c == '\r')
			{
				/*MSG_PEEK参数是为了防止下一个字符不是换行符,能够使该字符还保存在缓冲区中*/
				n = recv(iSock_fd,&c,1,MSG_PEEK);
				if(n > 0 && c == '\n')
				{
					recv(iSock_fd,&c,1,0);
					c = '\n';
				}
				else
					c = '\n';
			}
			aStr_tmp[i] = c;
			i++;
		}
		else
		{
			c = '\n';
		}
	}
	aStr_tmp[i] = '\0';
	
	return 0;
}

/*********************************************************
 ** 函数名  :   sig_chld( int signo) 
 ** 功能    :   SIGCHLD信号触发时调用此函数用于回收子进程
 ** 全局变量:
 ** 入口参数:	signo	HTTP服务器请求报文
 ** 返回值:
 ***********************************************************/
void sig_chld( int signo)
{
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0)
	{
		printf("child %d exit,exit_stat %d\n",pid,stat);
	}
	
	return;
}

/*********************************************************
 ** 函数名  :   nHttpInfoDeal(int iSock_fd)
 ** 功能    :   解析HTTP请求报文
 ** 全局变量:
 ** 入口参数:	iSock_fd	客户端socket描述符
 ** 返回值:
 ***********************************************************/
int nHttpInfoDeal(int iSock_fd) 
{
	int i = 0,ret;
	char aLine_end[2+1];
	char *start_tmp = NULL,*end_tmp = NULL;
	char aRcv_msg[2048+1];
	char aRsp_msg[2048+1];	/*应答信息*/
	ReqHeadMsg stReq_msg;
	HeadLineInfo stLine_info;
	
	memset(aLine_end,0x00,sizeof(aLine_end));
	memset(&stReq_msg,0x00,sizeof(stReq_msg));
	memset(aRcv_msg,0x00,sizeof(aRcv_msg));
	memset(aRsp_msg,0x00,sizeof(aRsp_msg));
	
	aLine_end[0] = 10;
	
	/*获取并解析起始行信息*/
	read_line(iSock_fd,aRcv_msg,sizeof(aRcv_msg) - 1);
	start_tmp = aRcv_msg;
	/*开始解析HTTP报文格式,起始行 <method> <request-URL> <version>,默认以回车换行结尾*/
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		printf("%d ERROR 解析method失败\n",__LINE__);
		return -1;
	}
	memcpy(stReq_msg.aMethod,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		printf("%d ERROR 解析request_url失败\n",__LINE__);
		return -1;
	}
	memcpy(stReq_msg.aRequest_url,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,'\n');
	if(end_tmp == NULL)
	{
		printf("%d ERROR 解析version失败\n",__LINE__);
		return -1;
	}
	memcpy(stReq_msg.aVersion,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + strlen(aLine_end);
	printf("method:[%s],URL:[%s],version:[%s]\n",stReq_msg.aMethod,stReq_msg.aRequest_url,stReq_msg.aVersion);
	
	/*至此,HTTP起始行的首部数据解析完成,开始解析首部块信息*/
	for(i = 0;;i++)
	{
		memset(aRcv_msg,0x00,sizeof(aRcv_msg));
		memset(&stLine_info,0x00,sizeof(stLine_info));
		read_line(iSock_fd,aRcv_msg,sizeof(aRcv_msg) - 1);
		if(strcmp(aRcv_msg,aLine_end) == 0)
			break;
		start_tmp = aRcv_msg;
		end_tmp = strchr(aRcv_msg,':');
		if(end_tmp == NULL)
		{
			printf("HTTP请求报文头非法,本行内容为:[%s]\n",aRcv_msg);
			return -1;
		}
		memcpy(stLine_info.aTitle,start_tmp,end_tmp - start_tmp);
		start_tmp = end_tmp + 2;
		end_tmp = strchr(start_tmp,'\n');
		memcpy(stLine_info.aMsg,start_tmp,end_tmp - start_tmp);
		if(strcmp(stLine_info.aTitle,"Content-length") == 0)
		{
			stReq_msg.iBody_len = atoi(stLine_info.aMsg);
		}
	}
	
	/*开始解析数据部分*/
	printf("HTTP请求报文中数据主体部分长度为:%d\n",stReq_msg.iBody_len);
	memset(aRcv_msg,0x00,sizeof(aRcv_msg));
	if(stReq_msg.iBody_len > 0)
		recv(iSock_fd,aRcv_msg,stReq_msg.iBody_len,0);
	
	
	/*封装应答信息*/
	sprintf(aRsp_msg,"HTTP/1.1 200 OK\n"
					 "Content-type: text/plain\n"
					 "Content-length: 19\n\n" 
					 "Hi! I'm a message!\n");
	printf("应答信息:[%s]\n",aRsp_msg);
	ret = send(iSock_fd,aRsp_msg,strlen(aRsp_msg),0);
	if(ret < 0)
	{
		printf("调用send函数失败,errno = %d,[%s]\n",errno,strerror(errno));
		return -1;
	}
	printf("响应信息发送完成\n");
	return 0;
}
