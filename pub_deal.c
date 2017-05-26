#include "pub_include.h"

/*向sockfd中写数据,数据长度为iLen,函数返回值为读取的实际长度*/
int sock_writen(int iSock_fd,char *aStr_tmp,int iLen)
{
	printf("进程号[%d],开始写入数据\n",getpid());
	int iByte_left = iLen;
	int iWrite_bytes = 0;
	char *ptr = aStr_tmp;
	
	if(iLen > strlen(aStr_tmp))
	{
		printf("传入参数长度[%d]大于传入字符串[%s]长度[%ld]\n",iLen,aStr_tmp,strlen(aStr_tmp));
		return -1;
	}
	
	while(iByte_left > 0)
	{
		iWrite_bytes = write(iSock_fd,ptr,iByte_left);
		if(iWrite_bytes <= 0)
		{
			if(errno = EINTR)
			{
				iWrite_bytes = 0;
			}
			else
			{
				printf("写数据失败,失败原因[%s]\n",strerror(errno));
				return -1;
			}
		}
		iByte_left -= iWrite_bytes;
		ptr = aStr_tmp + iWrite_bytes;
	}
	
	printf("进程号[%d],写入数据结束\n",getpid());
	return 0;
}

int sock_readn(int iSock_fd,char *aStr_tmp,int iLen)
{
	printf("进程号[%d],开始读取数据,待读取数据长度为[%d]\n",getpid(),iLen);
	int iByte_left = iLen;
	int iRead_len = 0;
	char *ptr = aStr_tmp;
	
	while(iByte_left > 0)
	{
		iRead_len = read(iSock_fd,ptr,iByte_left);
		printf("已读取长度为[%d]\n",iRead_len);
		if(iRead_len == -1)
		{
			if(errno = EINTR)
				iRead_len = 0;
			else
			{
				printf("读取报文失败,失败原因[%s]\n",strerror(errno));
				return -1;
			}
		}
		else if(iRead_len == 0)		/*已经读取不到数据*/
		{
			break;
		}
		iByte_left -= iRead_len;
		ptr += iRead_len;
	}
	
	printf("进程号[%d],读取数据结束\n",getpid());
	return 0;
}

void sig_chld( int signo)
{
	printf("receive sig_chld signal\n");
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0)
	{
		printf("child %d exit!\n",pid);
	}
	
	return;
}

/*********************************************************
 ** 函数名  :   nAnalyzeHttpRequestInfo(char *aRcv_msg) 
 ** 功能    :   解析HTTP请求报文
 ** 全局变量:
 ** 入口参数:	aRcv_msg	HTTP服务器请求报文
 				iMsg_len	请求报文长度   
 ** 返回值:
 ***********************************************************/
int nAnalyzeHttpRequestInfo(char *aRcv_msg,int iMsg_len) 
{
	int i = 0;
	char aLine_end[2+1];
	char *start_tmp = NULL,*end_tmp = NULL;
	ReqHttpMsg stReq_msg;
	
	memset(aLine_end,0x00,sizeof(aLine_end));
	memset(&stReq_msg,0x00,sizeof(stReq_msg));
	
	printf("接收到的请求信息为:[%s]\n",aRcv_msg);
	
	/*开始解析HTTP报文格式,起始行 <method> <request-URL> <version>,默认以回车换行结尾*/
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		printf("ERROR 解析method失败\n");
		return -1;
	}
	memcpy(stReq_msg.aMethod,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		printf("ERROR 解析request_url失败\n");
		return -1;
	}
	memcpy(stReq_msg.aRequest_url,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		printf("ERROR 解析version失败\n");
		return -1;
	}
	memcpy(stReq_msg.aVersion,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	printf("test\n");
	printf("method:[%s],URL:[%s],version:[%s]\n",stReq_msg.aMethod,stReq_msg.aRequest_url,stReq_msg.aVersion);
	
	return 0;
}
