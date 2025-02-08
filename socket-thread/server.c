#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 6500
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE] = {0};

void *worker(void *arg)
{
    int clientId = *(int *)arg;
    // sleep(1);
    char *body = "Hello from server\n";
    int bodyLength = strlen(body);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             bodyLength, body);

    int sendId = send(clientId, response, strlen(response), 0);
    if (sendId < 0)
    {
        perror("Send header fialed");
        close(clientId);
        free(arg);
        return NULL;
    }
    printf("Sent response: %s\n", body);
    int valread = read(clientId, buffer, sizeof(buffer));

    if (valread <= 0)
    {
        if (valread == 0)
        {
            perror("Client disconnected\n");
        }
        else
        {
            perror("Read failed");
        }
        close(clientId);
    }

    printf("Received from client %s\n", buffer);
    free(arg);
    close(clientId);
    return NULL;
};

int main()
{
    int serverId, *clientId;
    struct sockaddr_in address;
    int addlen = sizeof(address);

    serverId = socket(AF_INET, SOCK_STREAM, 0);
    if (serverId < 0)
    {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverId, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(serverId);
        exit(EXIT_FAILURE);
    }

    if (listen(serverId, 10) < 0)
    {
        perror("Listening failed");
        close(serverId);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1)
    {
        clientId = (int *)malloc(sizeof(int));
        *clientId = accept(serverId, (struct sockaddr *)&address, (socklen_t *)&addlen);
        if (*clientId < 0)
        {
            perror("Client acceptation failed");
            close(*clientId);
            free(clientId);
        }
        pthread_t th;
        printf("Address %p \n", clientId);
        if (pthread_create(&th, NULL, worker, clientId) != 0)
        {
            perror("Thread creation failed");
            close(*clientId);
        };
        // if (pthread_join(th, NULL) != 0)
        // {
        //     perror("Thread join failed");
        //     close(*clientId);
        // }
        pthread_detach(th);
    }
    close(serverId);
    return 0;
}