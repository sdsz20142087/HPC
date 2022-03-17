/*************************************************************************
	> File Name: ThreadPool.c
	> Author: 
	> Mail: 
	> Created Time: Fri 18 Feb 2022 12:41:50 AM CST
 ************************************************************************/

#include <stdio.h>
#include "head.h"
taskqueue* TaskQueue_init(taskqueue* tq, int n) {
	tq->head = 0, tq->tail = 0, tq->size = n, tq->total = 0;
	tq->data = (int *)malloc(sizeof(int) * n);
	pthread_mutex_init(&tq->tq_lock, NULL);
	pthread_cond_init(&tq->tq_cond1, NULL);
	pthread_cond_init(&tq->tq_cond2, NULL);
	return tq;
}

void TaskQueue_push(taskqueue* tq, int connfd) {
	pthread_mutex_lock(&tq->tq_lock);
	while (tq->total == tq->size) {
		//printf("Task Queue is full\n");
		pthread_cond_wait(&tq->tq_cond1, &tq->tq_lock);
	}
	tq->data[tq->tail] = connfd;
	tq->tail = (tq->tail + 1) % tq->size;
	tq->total++;
	pthread_cond_broadcast(&tq->tq_cond2);
	pthread_mutex_unlock(&tq->tq_lock);
	return ;
}

int TaskQueue_pop(taskqueue* tq) {
	pthread_mutex_lock(&tq->tq_lock);
	while (tq->total == 0) {
		//printf("Task Queue is empty\n");
		pthread_cond_wait(&tq->tq_cond2, &tq->tq_lock);
	}
	int connfd = tq->data[tq->head];
	tq->head = (tq->head + 1) % tq->size;
	tq->total--;
	pthread_cond_broadcast(&tq->tq_cond1);
	pthread_mutex_unlock(&tq->tq_lock);
	return connfd;
}