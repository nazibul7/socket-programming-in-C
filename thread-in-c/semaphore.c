#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREADS_IN_CRITICAL 3  // Only 3 threads allowed at a time

sem_t semaphore;
int count = 0;

void *routine(void *arg)
{
    int threadNo = *(int *)arg;
    printf("Thread %d is waiting to enter the critical section...\n", threadNo);

    sem_wait(&semaphore); // Wait for permission to enter critical section
    printf("Thread %d has entered the critical section.\n", threadNo);
    
    sleep(1); // Simulating critical section work
    count++;  // Increment shared count
    
    printf("Thread %d is leaving the critical section. Count: %d\n", threadNo, count);
    sem_post(&semaphore); // Release the semaphore

    free(arg);
    return NULL;
}

int main()
{
    pthread_t threads[10];

    // Initialize semaphore with MAX_THREADS_IN_CRITICAL
    sem_init(&semaphore, 0, MAX_THREADS_IN_CRITICAL);

    // Create 10 threads
    for (int i = 0; i < 10; i++)
    {
        int *ptr = (int *)malloc(sizeof(int));
        *ptr = i + 1;
        if (pthread_create(&threads[i], NULL, routine, ptr) != 0)
        {
            perror("Thread creation failed");
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Destroy the semaphore
    sem_destroy(&semaphore);

    printf("Final count: %d\n", count);
    return 0;
}
