/*************************************************************************
	> File Name: ThreadPool.h
	> Author: 
	> Mail: 
	> Created Time: Fri 18 Feb 2022 12:41:41 AM CST
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H
typedef struct TaskQueue{
	int head, tail, size, total, *data;
	pthread_mutex_t tq_lock;
	pthread_cond_t tq_cond1, tq_cond2;
}taskqueue;
taskqueue* TaskQueue_init(taskqueue* tq, int n);
void TaskQueue_push(taskqueue* tq, int connfd);
int TaskQueue_pop(taskqueue* tq);

#endif
