#include "pub_include.h"

/*********************************************************
 ** �ļ���  :   threadpool.c
 ** ����    :   �̳߳ش���
 ** �����߼�:   ������Ԥ������һ���������߳�,û��������ʱ,�̳߳��е��̴߳�������״̬
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
    
}