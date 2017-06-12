#include "pub_include.h"

/*********************************************************
 ** 文件名  :   threadpool.c
 ** 功能    :   线程池处理
 ** 处理逻辑:   本程序预先启动一定数量的线程,这些线程均运行一个函数,该函数从任务队列中获取任务,如果任务队列中没有
                任务,则线程池均处于挂起状态
 ***********************************************************/

/*********************************************************
 ** 函数名  :   threadpool_init(int thread_max_num, int queue_max_num)
 ** 功能    :   初始化线程池
 ** 全局变量:
 ** 入口参数:    thread_num         线程池中的线程数量
                 queue_max_num      任务队列的最大排队个数
 ** 出口参数:
                 
 ** 返回值:
 ***********************************************************/
struct threadpool* threadpool_init(int thread_num, int queue_max_num)
{
    struct threadpool *pool = NULL;
    int i,ret;
    do
    {
        pool = malloc(sizeof(struct threadpool));
        if(pool == NULL)
        {
            printf("申请空间失败\n");
            break;
        }
        
        pool->thread_max_num = thread_num;
        pool->queue_max_num = queue_max_num;
        pool->head = NULL;
        pool->tail = NULL;
        pool->pthreads = malloc(sizeof(pthread_t) * pool->thread_max_num);
        if(pthread_mutex_init(&pool->queue_lock,NULL))
        {
            printf("failed to init mutex!\n");
            break;
        }
        if(pthread_cond_init(&pool->queue_not_empty,NULL))
        {
            printf("failed to init queue_empty!\n");
            break;
        }
        if(pthread_cond_init(&pool->queue_full,NULL))
        {
            printf("failed to init queue_not_full!\n");
            break;
        }
        pool->cur_job_num = 0;
        
        /*启动线程*/
        for(i = 0;i < pool->thread_max_num;i++)
        {
            ret = pthread_create(&pool->pthreads[i],NULL,threadpool_function,(void *)pool);
            if(ret)
            {
                printf("创建新线程失败,errno = %d,[%s]\n",errno,strerror(errno));
                break;
            }
        }
        
        return pool;
    }while(0);
    
    return NULL;
}

/*********************************************************
 ** 函数名  :   threadpool_add_job(struct threadpool *pool,void*(*callback_function)(void *arg),void *arg)
 ** 功能    :   向线程池中添加任务
 ** 全局变量:
 ** 入口参数:    pool                   线程池地址
                 callback_function      线程回调函数
                 arg                    回调函数入口参数
 ** 出口参数:    无
                 
 ** 返回值:      0:成功 1:失败
 ***********************************************************/
int threadpool_add_job(struct threadpool *pool,void*(*callback_function)(void *arg),void *arg)
{
    pthread_mutex_lock(&pool->queue_lock);
    struct job *job_node = NULL;
    printf("添加任务,添加前任务队列中任务个数:%d\n",pool->cur_job_num);
    while(pool->cur_job_num == pool->queue_max_num)
    {
    	printf("任务队列已满,处于阻塞状态\n");
//        printf("the job queue is full,max job number %d\n",pool->queue_max_num);
        pthread_cond_wait(&pool->queue_full,&pool->queue_lock);
    }
    /*任务队列不满,开始向任务队列添加任务*/
    job_node = (struct job*)malloc(sizeof(struct job));
    if(job_node == NULL)
    {
        printf("call malloc fail\n");
        return -1;
    }
    job_node->callback_func = callback_function;
    job_node->arg = arg;
    
    job_node->next = NULL;
    
    if(pool->cur_job_num == 0)		/*如果任务队列为空*/
    {
    	pool->tail = job_node;
    	pool->head = job_node;
    }
    else
    {
    	pool->tail->next = job_node;
    	pool->tail = job_node;
    }
    pool->cur_job_num++;
	
    pthread_mutex_unlock(&pool->queue_lock);
    
    pthread_cond_signal(&pool->queue_not_empty);
    return 0;
}

/*********************************************************
 ** 函数名  :   threadpool_function(void *arg)
 ** 功能    :   线程处理函数,从任务队列中获取任务进行处理
 ** 全局变量:
 ** 入口参数:    
 ** 出口参数:    无
                 
 ** 返回值:
 ***********************************************************/
void *threadpool_function(void *arg)
{
    struct threadpool *pool = (struct threadpool*)arg;
    struct job *job_node = NULL;
    while(1)
    {
        /*获取任务队列互斥锁*/
        pthread_mutex_lock(&pool->queue_lock);
        while(pool->cur_job_num == 0)
        {
            /*如果任务队列中的任务数为0,则线程阻塞*/
            printf("thread [%lu] is waiting\n",pthread_self());
            pthread_cond_wait(&pool->queue_not_empty,&pool->queue_lock);
        }
        
        /*任务队列中有待处理的任务,从任务队列中获取任务并执行任务*/
        pool->cur_job_num--;
        job_node = pool->head;
        pool->head = pool->head->next;
        if(pool->head == NULL)
        {
            pool->tail = NULL;
        }
        pthread_mutex_unlock(&pool->queue_lock);
        
        /*job_node指向需要执行任务的节点,下面执行任务*/
        (*(job_node->callback_func))(job_node->arg);
        free(job_node);
        job_node = NULL;
        
        pthread_cond_signal(&pool->queue_full);
    }
}

/*********************************************************
 ** 函数名  :   threadpool_destroy(struct threadpool *pool)
 ** 功能    :   销毁线程池
 ** 全局变量:
 ** 入口参数:    
 ** 出口参数:    无
                 
 ** 返回值:
 ***********************************************************/
int threadpool_destroy(struct threadpool *pool)
{
	int i;
	
	if(pool->shutdown == 1)
		return -1;
	
	pool->shutdown = 1;
	
	/*唤醒所有等待线程*/
	pthread_cond_broadcast(&pool->queue_not_empty);
	
	for(i = 0;i < pool->thread_max_num;i++)
	{
		pthread_join(pool->pthreads[i],NULL);
	}
	free(pool->pthreads);
	
	pthread_mutex_destroy(&pool->queue_lock);
	pthread_cond_destroy(&pool->queue_not_empty);
	pthread_cond_destroy(&pool->queue_full);
	
	free(pool);
	
	pool = NULL;
	
	return 0;
}