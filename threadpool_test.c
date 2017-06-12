#include "pub_include.h"
void *my_process();

int main()
{
	struct threadpool *thread_pool = NULL;
	int i;
	int *b = (int *)malloc(sizeof(int));
	
	/*初始化线程池*/
	thread_pool = threadpool_init(10,20);
	if(thread_pool == NULL)
	{
		printf("线程池创建失败\n");
		return -1;
	}
	
	printf("线程池创建成功\n");
	
	printf("线程池线程数:%d,任务队列最大数目:%d,任务队列当前任务个数:%d\n",
			thread_pool->thread_max_num,thread_pool->queue_max_num,thread_pool->cur_job_num);
	
	/*向线程池添加任务*/
	for(i = 0;i < 35;i++)
	{
		*b = i;
		threadpool_add_job(thread_pool,my_process,b);
		sleep(1);
	}
	
	sleep(100);
	return 0;
}

void *my_process(void *arg)
{
	printf("thread is %lu,working on task %d\n",pthread_self(),*(int *)arg);
	sleep(100);
	return NULL;
}