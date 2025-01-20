#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 6000

int main()
{
    int server_id, client_id;
    struct sockaddr_in address;
    int addlen = sizeof(address);
    char buffer[1024] = {0};
    char *header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 17\r\n"
        "Connection: close\r\n"
        "\r\n";
    char *body = "Hello from server!";
    // Creating a socket
    server_id = socket(AF_INET, SOCK_STREAM, 0);
    if (server_id < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // initalizing memory for address variable
    memset(&address, 0, sizeof(address));

    // intialize address structer

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // ** despit define address as specific structure struct sockaddr_in for IPv4 address bind fucntion expect to work with multiple protocols, they work with the generic struct sockaddr type.

    if (bind(server_id, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_id);
        exit(EXIT_FAILURE);
    }

    // listening function

    if (listen(server_id, 10) < 0)
    {
        perror("Listeing failed");
        close(server_id);
        exit(EXIT_FAILURE);
    }
    printf("Server is listeing on port %d\n", PORT);

    // server_id param tells the operating system which listening socket should handle the incoming connection.
    // address param even though you initialized it earlier for the server, during accept(), it serves as an output parameter to store client information (IP +PORT).
    // It ensures that the server provides sufficient space for the client’s address and allows accept() to update the size of the actual address structure used.
    while (1)
    {
        client_id = accept(server_id, (struct sockaddr *)&address, (socklen_t *)&addlen);
        if (client_id < 0)
        {
            perror("Accept failed");
            close(client_id);
        }
        printf("Connection accepted from IP: %s, Port: %d\n",
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        // Buffer is a character array (or buffer) where the data read from the socket will be stored.
        // In this case, buffer is a pre-allocated array of 1024 bytes (char buffer[1024]), which means it can hold up to 1024 characters (or bytes) of incoming data.

        sleep(8);

        int valread = read(client_id, buffer, 1024-1);
        if (valread <= 0)
        {
             if (valread == 0) {
                printf("Client disconnected\n");
            } else {
                perror("Read failed");
            }
            close(client_id);
        }

        printf("Received from client: %s\n", buffer);

        if (send(client_id, header, strlen(header), 0) < 0)
        {
            perror("Send header failed");
            close(client_id);
        }
        if (send(client_id, body, strlen(body), 0) < 0)
        {
            perror("Send body failed");
            close(client_id);
        }

        close(client_id);
    }
    close(server_id);
    return 0;
}