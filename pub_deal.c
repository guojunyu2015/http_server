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
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0)
	{
		printf("child %d exit!\n",pid);
	}
	
	return;
}
