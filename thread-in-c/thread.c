#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>

int mail = 0;
pthread_mutex_t lock;
void *routine(void *arg)
{
    char *message = (char *)arg;
    printf("%s\n", message);
    pthread_mutex_lock(&lock);
    mail = mail + 1;
    pthread_mutex_unlock(&lock);
    int *mailPtr=malloc(sizeof(int));
    *mailPtr=mail;
    return mailPtr;
}

int main()
{
    pthread_t th[3];
    char *message = "Hello from thread";
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < 3; i++)
    {
        if (pthread_create(&th[i], NULL, routine, message) != 0)
        {
            perror("Thread creation failed");
            return 1;
        }
        printf("Thread creation done\n");
    }
    void *result;
    for (int i = 0; i < 3; i++)
    {
        if (pthread_join(th[i], &result) != 0)
        {
            perror("Thread join failed");
            return 1;
        }
        int *ptr=(int*)result;
        printf("Output is %d\n",*ptr);
    free(result);
        printf("Thread execution done\n");
    }
    pthread_mutex_destroy(&lock);
    printf("Total mail is %d\n", mail);
    return 0;
}