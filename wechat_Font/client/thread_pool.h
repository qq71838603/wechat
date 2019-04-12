#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>

#define MAX_WAITING_TASKS	1000
#define MAX_ACTIVE_THREADS	20

struct task
{
	void *(*task)(void *arg);
	void *arg;

	struct task *next;
};

typedef struct thread_pool
{
	pthread_mutex_t lock;
	pthread_cond_t  cond;
	struct task *task_list;

	pthread_t *tids;

	unsigned waiting_tasks;
	unsigned active_threads;

	bool shutdown;
}thread_pool;


bool
init_pool(thread_pool *pool,
          unsigned int threads_number);

bool
add_task(thread_pool *pool,
         void *(*task)(void *arg),
         void *arg);


bool destroy_pool(thread_pool *pool);
void *routine(void *arg);

#endif
