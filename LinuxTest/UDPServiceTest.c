#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8888
#define BUFFERSIZE 1024
#define MAX_CLIENTS 10
int main() {
    int server_fd, client_fd[MAX_CLIENTS];
    struct sockaddr_in servr_addr, client_addr;
    fd_set readfds;
    int max_fd, client_count = 0;
    char buffer[BUFFERSIZE];
    int addrlen = sizeof(client_addr);

    //创建socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) 
    {
        perror("socket");
        exit(1);
    }

    memset(&servr_addr, 0, sizeof(servr_addr));
    servr_addr.sin_family = AF_INET;
    servr_addr.sin_addr.s_addr = INADDR_ANY;
    servr_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&servr_addr, sizeof(servr_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("UDP 服务器启动，监听端口 %d...\n", PORT);
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        client_fd[i] = -1;
    }

    while (1) 
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        // 添加所有客户端 fd
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            if (client_fd[i] != -1) 
            {
                FD_SET(client_fd[i], &readfds);
                if (client_fd[i] > max_fd) max_fd = client_fd[i];
            }
        }

        // 等待事件
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) 
        {
            perror("select");
            exit(1);
        }

        // 新客户端连接
        if (FD_ISSET(server_fd, &readfds)) 
        {
            client_fd[client_count] = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
            if (client_fd[client_count] < 0) 
            {
                perror("accept");
                continue;
            }

            printf("新客户端连接: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            client_count++;

            // 广播欢迎消息
            sprintf(buffer, "欢迎新用户加入！当前在线: %d 人\n", client_count);
            for (int i = 0; i < client_count; i++) 
            {
                if (client_fd[i] != -1) 
                {
                    send(client_fd[i], buffer, strlen(buffer), 0);
                }
            }
        }

        for (int i = 0; i < client_count; i++) 
        {
            if (client_fd[i] != -1 && FD_ISSET(client_fd[i], &readfds)) 
            {
                int n = recv(client_fd[i], buffer, BUFFERSIZE, 0);
                if (n <= 0) 
                {
                    // 客户端断开
                    printf("客户端断开: %d\n", client_fd[i]);
                    close(client_fd[i]);
                    client_fd[i] = -1;
                    client_count--;
                } 
                else 
                {
                    buffer[n] = '\0';
                    printf("收到消息: %s", buffer);

                    // 广播给所有其他客户端
                    for (int j = 0; j < client_count; j++) 
                    {
                        if (client_fd[j] != -1 && client_fd[j] != client_fd[i]) 
                        {
                            send(client_fd[j], buffer, n, 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}