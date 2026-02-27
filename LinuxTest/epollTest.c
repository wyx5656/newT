#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8888
#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024

int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int listen_fd, epfd, nfds;
    struct sockaddr_in server_addr, client_addr;
    struct epoll_event ev, events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_nonblock(listen_fd);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_fd, 128);

    epfd = epoll_create1(0);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("epoll ET 服务器启动，监听端口 %d...\n", PORT);

    while (1) {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {  // 新连接
                while (1) {  // ET 模式必须循环 accept
                    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);
                    if (client_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        continue;
                    }

                    set_nonblock(client_fd);

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                    printf("新客户端连接: fd=%d\n", client_fd);
                }
            } else {  // 客户端数据到来
                while (1) {  // ET 模式必须循环读完
                    memset(buffer, 0, BUFFER_SIZE);
                    int n = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                    if (n == 0) {  // 客户端关闭
                        printf("客户端 fd=%d 断开\n", fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        break;
                    }

                    if (n < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // 读完
                        perror("recv");
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        break;
                    }

                    buffer[n] = '\0';
                    printf("收到 (fd=%d): %s", fd, buffer);

                    // 回显
                    send(fd, buffer, n, 0);
                }
            }
        }
    }

    close(listen_fd);
    close(epfd);
    return 0;
}