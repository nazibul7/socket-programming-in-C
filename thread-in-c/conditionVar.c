#include <stdio.h>
#include <pthread.h>
#include<unistd.h>

pthread_mutex_t lock;
pthread_cond_t cond_produce;
pthread_cond_t cond_consume;
#define bufferSize 5
int buffer[bufferSize];
int count = 0;

void *producer(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&lock);
        while (count == bufferSize)
        {
            pthread_cond_wait(&cond_produce, &lock);
        }
        buffer[count]=i;
        printf("Produced: %d\n", i);
        count++;

        pthread_cond_signal(&cond_consume);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
}

void *consumer(void *arg)
{
    for(int i=0;i<10;i++){
        pthread_mutex_lock(&lock);
        if (count==0)
        {
            pthread_cond_wait(&cond_consume,&lock);
        }
        int item=buffer[count-1];
        count--;
        printf("Consumed: %d\n", item);
        pthread_cond_signal(&cond_produce);
        pthread_mutex_unlock(&lock);
        sleep(2);
    }
}

int main()
{
    pthread_t producer_thread, consumer_thread;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond_produce, NULL);
    pthread_cond_init(&cond_consume, NULL);

    if (pthread_create(&producer_thread, NULL, producer, NULL) != 0)
    {
        perror("Producer thread creation failed");
        return 1;
    }
    if (pthread_create(&consumer_thread, NULL, consumer, NULL) != 0)
    {
        perror("Consumer thread creation failed");
        return 1;
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    pthread_cond_destroy(&cond_produce);
    pthread_cond_destroy(&cond_consume);
    pthread_mutex_destroy(&lock);
    return 0;
}