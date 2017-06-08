#include "pub_include.h"

/*********************************************************
 ** 文件名  :   threadpool.c
 ** 功能    :   线程池处理
 ** 处理逻辑:   本程序预先启动一定数量的线程,没有任务到来时,线程池中的线程处于阻塞状态
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
    struct threadpool *poll = NULL;
    int i,ret;
    do
    {
        poll = malloc(sizeof(struct threadpoll));
        if(poll == NULL)
        {
            printf("申请空间失败\n");
            return -1;
        }
        
        poll->thread_num = thread_num;
        poll->queue_max_num = queue_max_num;
        poll->head = NULL;
        poll->tail = NULL;
        poll->pthreads = malloc(sizeof(pthread_t) * poll->thread_num);
        if(pthread_mutex_init(&poll->mutex,NULL))
        {
            printf("failed to init mutex!\n");
            return -1;
        }
        if(pthread_cond_init(&poll->queue_empty,NULL))
        {
            printf("failed to init queue_empty!\n");
            break;
        }
        if(pthread_cond_init(&poll->queue_not_empty,NULL))
        {
            printf("failed to init queue_not_empty!\n");
            break;
        }
        if(pthread_cond_init(&poll->queue_not_full,NULL))
        {
            printf("failed to init queue_not_full!\n");
            break;
        }
        poll->queue_cur_num = 0;
        
        /*启动线程*/
        for(i = 0;i < poll->thread_num;i++)
        {
            ret = pthread_create(&pool->pthreads[i],NULL,threadpoll_function,(void *)poll);
            if(ret)
            {
                printf("创建新线程失败,errno = %d,[%s]\n",errno,strerror(errno));
                break;
            }
        }
        
        return poll;
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
    
}