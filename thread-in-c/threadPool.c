#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<unistd.h>
#include<sched.h>

#define THREAD_POOL_SIZE 5
#define TASK_QUEUE_SIZE 10

pthread_mutex_t lock;
pthread_cond_t condVar;

typedef struct
{
    int x, y;
} Task;

Task taskQueue[TASK_QUEUE_SIZE];
int count = 0;

void executeTask(Task *task)
{
    // sleep(1);
    int result = task->x + task->y;
    printf("The sum of %d and %d is %d by thread %ld\n", task->x, task->y, result,pthread_self());
    sched_yield(); 
}

void *worker(void *arg)
{
    while (1)
    {
        // Task task;

        pthread_mutex_lock(&lock);
        while (count == 0)
        {
            printf("Waiting for task\n");
            pthread_cond_wait(&condVar, &lock);
        }

        Task task = taskQueue[0];
        for (int i = 0; i < count - 1; i++)
        {
            taskQueue[i] = taskQueue[i + 1];
        }
        count--;
        pthread_mutex_unlock(&lock);
        executeTask(&task);
    }
    return NULL;
}

int main()
{
    // Initialize the thread
    pthread_t thread[THREAD_POOL_SIZE];

    // Initalize mutex & cond var
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&condVar, NULL);

    // Thread creating
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_create(&thread[i], NULL, worker, NULL) != 0)
        {
            perror("Thread creation failed");
            return 1;
        }
    }

    // Assign the task in task queue
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&lock);
        taskQueue[count].x = i + 1;
        taskQueue[count].y = i + 1;
        count++;
        pthread_cond_signal(&condVar);
        pthread_mutex_unlock(&lock);
    }

    // Thread join to finished properly
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_join(thread[i], NULL) != 0)
        {
            perror("Thread join failed");
            return 1;
        }
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condVar);
    return 0;
}
