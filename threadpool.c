#include "pub_include.h"

/*********************************************************
 ** �ļ���  :   threadpool.c
 ** ����    :   �̳߳ش���
 ** �����߼�:   ������Ԥ������һ���������߳�,��Щ�߳̾�����һ������,�ú�������������л�ȡ����,������������û��
                ����,���̳߳ؾ����ڹ���״̬
 ***********************************************************/

/*********************************************************
 ** ������  :   threadpool_init(int thread_max_num, int queue_max_num)
 ** ����    :   ��ʼ���̳߳�
 ** ȫ�ֱ���:
 ** ��ڲ���:    thread_num         �̳߳��е��߳�����
                 queue_max_num      ������е�����ŶӸ���
 ** ���ڲ���:
                 
 ** ����ֵ:
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
            printf("����ռ�ʧ��\n");
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
        
        /*�����߳�*/
        for(i = 0;i < poll->thread_num;i++)
        {
            ret = pthread_create(&pool->pthreads[i],NULL,threadpoll_function,(void *)poll);
            if(ret)
            {
                printf("�������߳�ʧ��,errno = %d,[%s]\n",errno,strerror(errno));
                break;
            }
        }
        
        return poll;
    }while(0);
    
    return NULL;
}

/*********************************************************
 ** ������  :   threadpool_add_job(struct threadpool *pool,void*(*callback_function)(void *arg),void *arg)
 ** ����    :   ���̳߳����������
 ** ȫ�ֱ���:
 ** ��ڲ���:    pool                   �̳߳ص�ַ
                 callback_function      �̻߳ص�����
                 arg                    �ص�������ڲ���
 ** ���ڲ���:    ��
                 
 ** ����ֵ:      0:�ɹ� 1:ʧ��
 ***********************************************************/
int threadpool_add_job(struct threadpool *pool,void*(*callback_function)(void *arg),void *arg)
{
    pthread_mutex_lock(&pool->queue_lock);
    struct job *job_node = NULL;
    while(pool->cur_job_num == pool->queue_max_num)
    {
        printf("the job queue is full,max job number %d\n",pool->queue_max_num);
        pthread_cond_wait(&pool->queue_full,&pool->queue_lock);
    }
    
    /*������в���,��ʼ����������������*/
    job_node = (struct job*)malloc(sizeof(struct job));
    if(job_node == NULL)
    {
        printf("call malloc fail\n");
        return -1;
    }
    
    job_node->next = NULL;
    pool->tail->next = job_node;
    pool->tail = job_node;
    if(pool->head == NULL)
    {
        pool->head = job_node;
    }
    pool->cur_job_num++;

    pthread_mutex_unlock(&pool->queue_lock);
    
    pthread_cond_signal(&pool->queue_not_empty);
}

/*********************************************************
 ** ������  :   threadpoll_function(void *arg)
 ** ����    :   �̴߳�����,����������л�ȡ������д���
 ** ȫ�ֱ���:
 ** ��ڲ���:    
 ** ���ڲ���:    ��
                 
 ** ����ֵ:
 ***********************************************************/
void *threadpoll_function(void *arg)
{
    struct threadpool *pool = (struct threadpool*)arg;
    struct job *job_node = NULL;
    while(1)
    {
        /*��ȡ������л�����*/
        pthread_mutex_lock(pool->queue_lock);
        while(pool->cur_job_num == 0)
        {
            /*�����������е�������Ϊ0,���߳�����*/
            printf("thread [%d] is waiting\n",pthread_self());
            pthread_cond_wait(&pool->queue_not_empty,&pool->queue_lock);
        }
        
        /*����������д����������,����������л�ȡ����ִ������*/
        pool->cur_job_num--;
        job_node = pool->head;
        pool->head = pool->head->next;
        if(pool->head == NULL)
        {
            pool->tail = NULL;
        }
        pthread_mutex_unlock(&pool->queue_lock);
        
        /*job_nodeָ����Ҫִ������Ľڵ�,����ִ������*/
        (*(job_node->callback_func))(job->arg);
        free(job_node);
        job_node = NULL;
        
        pthread_cond_signal(&pool->queue_full);
    }
}
