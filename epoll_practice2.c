#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 7000
#define MAX_EVENTS 100
#define BUFFER_SIZE 1024

int set_nonblocking(int fd)
{
    /**
     * This reads the current file descriptor flags. A socket might already have other flags.
     * If you set new flags blindly, we might unintentionally remove important settings already present.
     */
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return -1;
    }

    /**
     * The | (bitwise OR) adds O_NONBLOCK to the existing flags without removing the others.
     * If we skip first one & set fcntl(fd, F_SETFL, O_NONBLOCK);  // WRONG!
     * Because we'd wipe out all previous settings and set only non-blocking mode.
     * By using flags | O_NONBLOCK, you turn on non-blocking mode without disturbing other settings.
     */
    int updatedFlags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (updatedFlags == -1)
    {
        perror("fcntl F_SETFL O_NONBLOCK");
        return -1;
    }
    return 0;
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addlen = sizeof(address);
    // creating socket for server_fd
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Make server socket non-blocking
    if (set_nonblocking(server_fd) == -1)
    {
        perror("Failed to set server socket non-blocking");
        exit(EXIT_FAILURE);
    }

    // Reuse address
    int one = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    // set default value of the address var to 0
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind server socket fd with address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed to bind server socet fd & address");
        exit(EXIT_FAILURE);
    }

    // listening on server fd

    if (listen(server_fd, 128) < 0)
    {
        perror("Failed to listen on server fd");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on PORT %d\n", PORT);

    // initialize epoll fd

    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0)
    {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }

    /**
     * struct epoll_event event is just a C struct that you fill out to tell the epoll system call:
     * - Which events to monitor (event.events)
     * - Which user data should be returned to u when those events happens
     */

    struct epoll_event event, events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = server_fd;

    /**
     * epoll_ctl() → Add/modify/remove an fd from the kernel’s watchlist.
     */

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0)
    {
        perror("epoll_ctl: EPOLL_CTL_ADD server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        /**
         * epoll_wait() → Suspend the calling thread until one or more fds in the kernel’s watchlist become ready,
         * or a timeout occurs.
         */

        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue; // Interrupted by signal — just restart epoll_wait
            else
            {
                perror("epoll_wait failed");
                break;
            }
        }

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == server_fd)
            {
                while (1)
                {
                    client_fd = accept(server_fd, (struct sockaddr *)&address, &addlen);

                    if (client_fd < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        else if (errno == EINTR)
                        {
                            continue; // interrupted by signal, retry accept
                        }
                        else
                        {
                            perror("accept failed");
                            break;
                        }
                    }

                    if (set_nonblocking(client_fd) == -1)
                    {
                        perror("Failed to set non blocking mode.");
                        close(client_fd);
                        continue;
                    }

                    printf("Connection from %s:%d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    event.events = EPOLLIN;
                    event.data.fd = client_fd;

                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event))
                    {
                        perror("epoll_ctl: EPOLL_CTL_ADD failed");
                        close(client_fd);
                        continue;
                    }
                }
            }
            else
            {
                char buffer[BUFFER_SIZE];
                while (1)
                {
                    int current_fd = events[i].data.fd;
                    ssize_t bytes_read = read(current_fd, buffer, BUFFER_SIZE - 1);

                    if (bytes_read == 0)
                    {
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_fd, &event);
                        close(current_fd);
                        break;
                    }
                    else if (bytes_read > 0)
                    {
                        buffer[bytes_read] = '\0';
                        printf("Received request: %s\n", buffer);
                        char http_response[] =
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Connection: close\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello World!\n";
                        write(current_fd, http_response, strlen(http_response));
                        printf("Sent HTTP response to client: %d\n", current_fd);
                        close(current_fd);
                        break;
                    }
                    else
                    {
                        /**
                         * EAGIN or EWOULDBLOCK will only come for non-blocking socket not for blocking socket.
                         */
                        if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        else
                        {
                            perror("read error");
                            close(events[i].data.fd);
                            break;
                        }
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}