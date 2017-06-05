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
void * nHttpInfoDeal(void  *t) 
{
	int i = 0,ret;
	char aLine_end[2+1];
	char *start_tmp = NULL,*end_tmp = NULL;
	char aRcv_msg[2048+1];
	char aRsp_msg[2048+1];	/*应答信息*/
	int  iSock_fd;
	ReqHeadMsg stReq_msg;
	HeadLineInfo stLine_info;
	char aLog_filenm[64];
	struct timeval t_start,t_end;
	double fRun_time = 0.00;	/*运行时间*/
	
	/*获取运行起始时间*/
	gettimeofday(&t_start,NULL);
	
	memset(aLine_end,0x00,sizeof(aLine_end));
	memset(&stReq_msg,0x00,sizeof(stReq_msg));
	memset(aRcv_msg,0x00,sizeof(aRcv_msg));
	memset(aRsp_msg,0x00,sizeof(aRsp_msg));
	memset(aLog_filenm,0x00,sizeof(aLog_filenm));
	
	/*封装日志文件名*/
	sprintf(aLog_filenm,"thread_%lu",pthread_self());
	iSock_fd = (int)t;
	aLine_end[0] = 10;
	
	bsWPubDebug(3,aLog_filenm,"%d 接收到客户端请求,客户端socket描述符:[%d]",__LINE__,iSock_fd);
	
	/*获取并解析起始行信息*/
	read_line(iSock_fd,aRcv_msg,sizeof(aRcv_msg) - 1);
	start_tmp = aRcv_msg;
	/*开始解析HTTP报文格式,起始行 <method> <request-URL> <version>,默认以回车换行结尾*/
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		bsWPubDebug(3,aLog_filenm,"%d ERROR 解析method失败",__LINE__);
		return (void *)-1;
	}
	memcpy(stReq_msg.aMethod,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,' ');
	if(end_tmp == NULL)
	{
		bsWPubDebug(3,aLog_filenm,"%d ERROR 解析request_url失败",__LINE__);
		return (void *)-1;
	}
	memcpy(stReq_msg.aRequest_url,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + 1;
	
	end_tmp = strchr(start_tmp,'\n');
	if(end_tmp == NULL)
	{
		bsWPubDebug(3,aLog_filenm,"%d ERROR 解析version失败n",__LINE__);
		return (void *)-1;
	}
	memcpy(stReq_msg.aVersion,start_tmp,end_tmp - start_tmp);
	start_tmp = end_tmp + strlen(aLine_end);
	bsWPubDebug(3,aLog_filenm,"method:[%s],URL:[%s],version:[%s]",stReq_msg.aMethod,stReq_msg.aRequest_url,stReq_msg.aVersion);
	
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
			bsWPubDebug(3,aLog_filenm,"HTTP请求报文头非法,本行内容为:[%s]",aRcv_msg);
			return (void *)-1;
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
	bsWPubDebug(3,aLog_filenm,"HTTP请求报文中数据主体部分长度为:%d",stReq_msg.iBody_len);
	memset(aRcv_msg,0x00,sizeof(aRcv_msg));
	if(stReq_msg.iBody_len > 0)
		recv(iSock_fd,aRcv_msg,stReq_msg.iBody_len,0);
	
	
	nDealHttpMethod(stReq_msg,aRsp_msg);
	bsWPubDebug(3,aLog_filenm,"响应信息:[%s]",aRsp_msg);
	
	ret = send(iSock_fd,aRsp_msg,strlen(aRsp_msg),0);
	if(ret < 0)
	{
		bsWPubDebug(3,aLog_filenm,"调用send函数失败,errno = %d,[%s]",errno,strerror(errno));
		return (void *)-1;
	}
	bsWPubDebug(3,aLog_filenm,"响应信息发送完成");
	close(iSock_fd);
	
	/*获取运行结束时间*/
	gettimeofday(&t_end,NULL);
	
	/*计算运行时间*/
	fRun_time = (t_end.tv_sec - t_start.tv_sec) * 1000 + (t_end.tv_usec - t_start.tv_usec) / 1000;
	bsWPubDebug(3,aLog_filenm,"运行时间[%.3f]毫秒",fRun_time);
	return (void *)0;
}

/*********************************************************
 ** 函数名  :   nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info)
 ** 功能    :   处理HTTP请求信息并生成HTTP应答信息
 ** 全局变量:
 ** 入口参数:	head_msg	HTTP请求首行信息
 	出口参数:	rsp_info HTTP响应信息(包含了报文头和报文体)
 ** 返回值:		0:成功 -1:失败
 ***********************************************************/
int nDealHttpMethod(ReqHeadMsg head_msg,char *rsp_info)
{
	char aStr_tmp[1024];
	char aFile_name[128];
	char aFile_dtl[2048];
	FILE *fp = NULL;
	
	memset(aFile_name,0x00,sizeof(aFile_name));
	memset(aStr_tmp,0x00,sizeof(aStr_tmp));
	memset(aFile_dtl,0x00,sizeof(aFile_dtl));
	
	if(strcmp(head_msg.aRequest_url,"/") == 0)
		strncpy(aFile_name,"./docs/index.txt",sizeof(aFile_name) -1);
	
	if(strcmp(head_msg.aMethod,"GET") == 0)	/*如果为GET方法*/
	{
		fp = fopen(aFile_name,"r");
		if(fp == NULL)
		{
			printf("打开文件[%s]失败,errno = %d,[%s]\n",aFile_name,errno,strerror(errno));
			nResourceNotFound(head_msg,rsp_info);
			return 0;
		}
		while(!feof(fp))
		{
			memset(aStr_tmp,0x00,sizeof(aStr_tmp));
			fgets(aStr_tmp,sizeof(aStr_tmp)-1,fp);
			sprintf(aFile_dtl,"%s%s",aFile_dtl,aStr_tmp);
		}
	}
	
	/*封装HTTP应答报文*/
	sprintf(rsp_info,"%s 200 OK\r\n"
					 "Content-type: text/plain\r\n"
					 "Content-length: %d\r\n"
					 "\r\n"
					 "%s",head_msg.aVersion,(int)strlen(aFile_dtl),aFile_dtl);
	
	return 0;
}

/*********************************************************
 ** 函数名  :   nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info)
 ** 功能    :   如果请求资源不存在,生成资源不存在的应答信息
 ** 全局变量:
 ** 入口参数:	head_msg	HTTP请求首行信息
 	出口参数:	rsp_info HTTP响应信息(包含了报文头和报文体)
 ** 返回值:		0:成功 -1:失败
 ***********************************************************/
int nResourceNotFound(ReqHeadMsg head_msg,char *rsp_info)
{
	sprintf(rsp_info,"%s 400 Resource not exist /(ㄒoㄒ)/~~\r\n"
					 "Content-type: text/html\r\n"
					 "\r\n"
					 "<P>Your browser sent a bad request\r\n",
					 head_msg.aVersion);
	return 0;
}

/*********************************************************
 ** 函数名  :   nAnalyseCfgFilePubDeal(char *aConfig_desc,char *aConfig_str) 
 ** 功能    :   解析配置文件内容
 ** 全局变量:
 ** 入口参数:   aConfig_desc：配置信息描述符
 	出口参数:	aConfig_str:配置详细信息
 ** 返回值:
 ***********************************************************/
int nAnalyseCfgFilePubDeal(char *aConfig_desc,char *aConfig_str)
{
	FILE *fp = NULL;
	char aStr_tmp[4096+1];
	char aConfig_tmp[128+1];
	long lStart_flag = 0; /*0-未起始 1-起始*/
	char *aStart_tmp,*aEnd_tmp;
	int  iMatch_flag = 0;	/*匹配标志:0-未匹配 1-已匹配*/
	fp = fopen("http_server.cfg","r");
	if (fp == NULL)
	{
		printf("open config file http_server.cfg fail\n");
		return -1;
	}
	while(feof(fp) == 0 && iMatch_flag == 0)
	{
		memset (aStr_tmp,0x00,sizeof(aStr_tmp));
		memset(aConfig_tmp,0x00,sizeof(aConfig_tmp));
		aStart_tmp = NULL;
		fgets(aStr_tmp,sizeof(aStr_tmp)-1,fp);
		aStart_tmp = strchr(aStr_tmp,'\n');
		if(aStart_tmp)
			aStr_tmp[strlen(aStr_tmp) - 1] = '\0';
		
		/*如果为注释行或空行,则跳到下一行处理*/
		if (aStr_tmp[0] == '#' || strlen(aStr_tmp) == 0)
			continue;
		
		aStart_tmp = aStr_tmp;
		aEnd_tmp = strchr(aStr_tmp,'=');
		if(aEnd_tmp == NULL)
		{
			printf("配置行非法,本行信息:[%s]\n",aStr_tmp);
			return -1;
		}
		
		memcpy(aConfig_tmp,aStart_tmp,aEnd_tmp-aStart_tmp);
		if(strcmp(aConfig_tmp,aConfig_desc) != 0)
			continue;
		
		aEnd_tmp++;
		strcpy(aConfig_str,aEnd_tmp);
		iMatch_flag = 1;
	}
	
	if (iMatch_flag == 0)
	{
		printf("config_type[%s] is not exist,please confirm\n",aConfig_desc);
		return -1;
	}
	return 0;
}

/*********************************************************
 ** 函数名  :   bsWPubDebug(char *aFile_name,char *fmt,...) 
 ** 功能    :   日志打印公共函数
 ** 全局变量:
 ** 参数含义:   
 ** 返回值:
 ***********************************************************/
void bsWPubDebug(int debug_level,char *aLog_file_name,char *fmt,...)
{
	FILE *fp;
	char aFile_name[64];
	va_list ap;
	char aStr_tmp[256];
	char aTime_stamp[64+1];		/*时间戳*/
	struct  tm *systime;
    time_t  t;
    struct timeval t_sec;
    
    /*判断当前配置的日志级别*/
    if(debug_level > atoi(sgTransConf.aDebug_level))
    	return;
    time(&t);
    systime = localtime(&t);
   	
    gettimeofday(&t_sec,NULL);
    sprintf(aTime_stamp,"%04d-%02d-%02d %02d:%02d:%02d.%d",
    					systime->tm_year+1900,systime->tm_mon+1,systime->tm_mday,
    					systime->tm_hour,systime->tm_min,systime->tm_sec,
    					t_sec.tv_usec);
	memset(aFile_name,0x00,sizeof(aFile_name));
	memset(aStr_tmp,0x00,sizeof(aStr_tmp));
	fp = NULL;
	sprintf(aFile_name,"./log/%s.log",aLog_file_name);
	fp = fopen(aFile_name,"a+");
	if(fp == NULL)
	{
		printf("打开文件[%s]失败",aFile_name);
		return;
	}
	va_start(ap,fmt);
	vsnprintf(aStr_tmp,sizeof(aStr_tmp),fmt,ap);
	va_end(ap);
	fprintf(fp,"[%s] %s\n",aTime_stamp,aStr_tmp);
	fclose(fp);
}
