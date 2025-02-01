#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<stdlib.h>
#include<time.h>

#define buffer_size 6

int buffer[buffer_size];
int count = 0;
sem_t empty, full, allow;

void *producer(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        sem_wait(&empty);
        sem_wait(&allow);
        int item = rand() % 100; // Generate a random number between 0 and 99
        buffer[count] = item;    // Produce an item
        printf("Produced: %d at index %d\n", item, count);
        count++;
        sem_post(&allow);
        sem_post(&full);
        sleep(1);
    }
}

void *consumer(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        sem_wait(&full);
        sem_wait(&allow);
        int item = buffer[count-1]; // Consume an item from the buffer
        count--;
        printf("Consumed: %d from index %d\n", item, count);
        sem_post(&allow);
        sem_post(&empty);
        sleep(1);
    }
}

int main()
{
    pthread_t th[buffer_size];
    sem_init(&empty, 0, buffer_size);
    sem_init(&full, 0, 0);
    sem_init(&allow, 0, 1);
    for (int i = 0; i < buffer_size; i++)
    {
        if (i % 2 == 0)
        {
            if (pthread_create(&th[i], NULL, producer, NULL) != 0)
            {
                perror("Thread creation fialed");
                return 1;
            }
        }
        else
        {
            if (pthread_create(&th[i], NULL, consumer, NULL) != 0)
            {
                perror("Thread creation fialed");
                return 1;
            }
        }
    }
    for (int i = 0; i < buffer_size; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Thread join failed");
            return 1;
        }
    }
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&allow);
    return 0;
}